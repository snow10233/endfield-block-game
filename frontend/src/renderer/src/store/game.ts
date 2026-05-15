import { ref, computed, type ComputedRef } from 'vue'
import { backend } from '../api/backend'
import type { GameState, MovablePiece } from '../types/level'

// Single shared reactive game-state module. Imported by any component that
// needs to read or mutate state — Vue's module-singleton behavior makes this
// effectively a tiny store without needing Pinia.

const state = ref<GameState | null>(null)
const lastError = ref<string | null>(null)
const lastErrorAt = ref(0)

async function refresh(): Promise<void> {
  const s = await backend.state()
  state.value = s
}

async function load(path: string): Promise<void> {
  const r = await backend.load(path)
  if (!r.ok) {
    lastError.value = r.error ?? 'load failed'
    lastErrorAt.value = Date.now()
    return
  }
  await refresh()
}

async function place(pieceId: number, row: number, col: number, rot: number): Promise<boolean> {
  const r = await backend.place(pieceId, row, col, rot)
  if (!r.ok) {
    lastError.value = r.error ?? 'place failed'
    lastErrorAt.value = Date.now()
    return false
  }
  lastError.value = null
  await refresh()
  return true
}

async function removePiece(pieceId: number): Promise<void> {
  await backend.remove(pieceId)
  await refresh()
}

async function reset(): Promise<void> {
  await backend.reset()
  await refresh()
}

function clear(): void {
  state.value = null
  lastError.value = null
  lastErrorAt.value = 0
}

export function useGame(): {
  state: typeof state
  lastError: typeof lastError
  lastErrorAt: typeof lastErrorAt
  unplacedPieces: ComputedRef<MovablePiece[]>
  placedPieces: ComputedRef<MovablePiece[]>
  load: typeof load
  refresh: typeof refresh
  place: typeof place
  removePiece: typeof removePiece
  reset: typeof reset
  clear: typeof clear
} {
  const unplacedPieces = computed(() => state.value?.pieces.filter((p) => !p.placed) ?? [])
  const placedPieces = computed(() => state.value?.pieces.filter((p) => p.placed) ?? [])
  return {
    state,
    lastError,
    lastErrorAt,
    unplacedPieces,
    placedPieces,
    load,
    refresh,
    place,
    removePiece,
    reset,
    clear
  }
}
