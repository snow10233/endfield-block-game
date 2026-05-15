import type { GameState, PlaceResponse, SimpleOk } from '../types/level'

async function call<T>(op: string, args: Record<string, unknown> = {}): Promise<T> {
  return (await window.api.backend.call(op, args)) as T
}

export const backend = {
  async load(path: string): Promise<SimpleOk> {
    return call('load', { path })
  },
  async state(): Promise<GameState & { id: number }> {
    return call('state')
  },
  async place(pieceId: number, row: number, col: number, rot: number): Promise<PlaceResponse> {
    return call('place', { pieceId, row, col, rot })
  },
  async remove(pieceId: number): Promise<SimpleOk> {
    return call('remove', { pieceId })
  },
  async reset(): Promise<SimpleOk> {
    return call('reset')
  }
}
