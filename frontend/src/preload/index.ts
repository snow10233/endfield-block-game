import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'

const api = {
  backend: {
    call(op: string, args: Record<string, unknown> = {}): Promise<unknown> {
      return ipcRenderer.invoke('backend:call', op, args)
    }
  },
  dialog: {
    openLevel(): Promise<string | null> {
      return ipcRenderer.invoke('dialog:openLevel')
    }
  }
}

if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', api)
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (define in dts)
  window.electron = electronAPI
  // @ts-ignore (define in dts)
  window.api = api
}
