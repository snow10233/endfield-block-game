import { ElectronAPI } from '@electron-toolkit/preload'

interface BackendApi {
  call(op: string, args?: Record<string, unknown>): Promise<unknown>
}

interface DialogApi {
  openLevel(): Promise<string | null>
}

declare global {
  interface Window {
    electron: ElectronAPI
    api: {
      backend: BackendApi
      dialog: DialogApi
    }
  }
}
