import { app, shell, BrowserWindow, ipcMain, dialog } from 'electron'
import { join, resolve } from 'path'
import { spawn, ChildProcessWithoutNullStreams } from 'child_process'
import { createInterface, Interface as ReadlineInterface } from 'readline'
import { electronApp, optimizer, is } from '@electron-toolkit/utils'
import icon from '../../resources/icon.png?asset'

type PendingRequest = {
  resolve: (value: unknown) => void
  reject: (err: Error) => void
}

class BackendBridge {
  private child: ChildProcessWithoutNullStreams | null = null
  private rl: ReadlineInterface | null = null
  private nextId = 1
  private pending = new Map<number, PendingRequest>()

  start(binaryPath: string, cwd: string): void {
    this.child = spawn(binaryPath, [], { cwd, stdio: ['pipe', 'pipe', 'pipe'] })

    this.child.stderr.on('data', (chunk: Buffer) => {
      process.stderr.write(`[backend] ${chunk.toString()}`)
    })

    this.child.on('exit', (code) => {
      console.log(`[backend] exited with code ${code}`)
      for (const [, pending] of this.pending) {
        pending.reject(new Error('backend exited'))
      }
      this.pending.clear()
    })

    this.rl = createInterface({ input: this.child.stdout })
    this.rl.on('line', (line) => this.handleLine(line))
  }

  private handleLine(line: string): void {
    try {
      const obj = JSON.parse(line) as { id?: number }
      if (typeof obj.id !== 'number') return
      const pending = this.pending.get(obj.id)
      if (!pending) return
      this.pending.delete(obj.id)
      pending.resolve(obj)
    } catch (err) {
      console.error('[backend] bad response line:', line, err)
    }
  }

  call(op: string, args: Record<string, unknown> = {}): Promise<unknown> {
    if (!this.child || this.child.killed)
      return Promise.reject(new Error('backend not running'))
    const id = this.nextId++
    const payload = JSON.stringify({ id, op, ...args }) + '\n'
    return new Promise((resolveFn, rejectFn) => {
      this.pending.set(id, { resolve: resolveFn, reject: rejectFn })
      this.child!.stdin.write(payload)
    })
  }

  kill(): void {
    if (this.child && !this.child.killed) {
      this.child.kill()
    }
  }
}

const backend = new BackendBridge()

function backendBinaryPath(): { bin: string; cwd: string } {
  const exe = process.platform === 'win32' ? 'main.exe' : 'main'
  if (app.isPackaged) {
    const base = join(process.resourcesPath, 'backend')
    return { bin: join(base, exe), cwd: base }
  }
  const backendDir = resolve(__dirname, '../../../backend')
  return { bin: join(backendDir, 'build', exe), cwd: join(backendDir, 'build') }
}

function createWindow(): void {
  const mainWindow = new BrowserWindow({
    width: 1100,
    height: 800,
    show: false,
    autoHideMenuBar: true,
    ...(process.platform === 'linux' ? { icon } : {}),
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false
    }
  })

  mainWindow.on('ready-to-show', () => {
    mainWindow.show()
  })

  mainWindow.webContents.setWindowOpenHandler((details) => {
    shell.openExternal(details.url)
    return { action: 'deny' }
  })

  if (is.dev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

app.whenReady().then(() => {
  electronApp.setAppUserModelId('com.electron')

  app.on('browser-window-created', (_, window) => {
    optimizer.watchWindowShortcuts(window)
  })

  const { bin, cwd } = backendBinaryPath()
  backend.start(bin, cwd)

  ipcMain.handle('backend:call', async (_event, op: string, args: Record<string, unknown>) => {
    return await backend.call(op, args)
  })

  ipcMain.handle('dialog:openLevel', async () => {
    const defaultPath = app.isPackaged
      ? join(process.resourcesPath, 'backend/tests')
      : resolve(__dirname, '../../../backend/tests')
    const { canceled, filePaths } = await dialog.showOpenDialog({
      title: '選擇關卡檔案',
      properties: ['openFile'],
      filters: [
        { name: 'Level files', extensions: ['txt'] },
        { name: 'All files', extensions: ['*'] }
      ],
      defaultPath
    })
    if (canceled || filePaths.length === 0) return null
    return filePaths[0]
  })

  ipcMain.handle('dialog:saveLevel', async (_event, text: string) => {
    const defaultPath = app.isPackaged
      ? join(process.resourcesPath, 'backend/tests', 'level.txt')
      : resolve(__dirname, '../../../backend/tests', 'level.txt')
    const { canceled, filePath } = await dialog.showSaveDialog({
      title: '匯出關卡',
      defaultPath,
      filters: [{ name: 'Level files', extensions: ['txt'] }]
    })
    if (canceled || !filePath) return null
    const { writeFile } = await import('fs/promises')
    await writeFile(filePath, text, 'utf-8')
    return filePath
  })

  createWindow()

  app.on('activate', function () {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('before-quit', () => {
  backend.kill()
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
