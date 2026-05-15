import { ref, computed, type ComputedRef } from 'vue'
import { backend } from '../api/backend'
import type { GameState, MovablePiece, SolverPlacement } from '../types/level'

// Single shared reactive game-state module. Imported by any component that
// needs to read or mutate state — Vue's module-singleton behavior makes this
// effectively a tiny store without needing Pinia.

const state = ref<GameState | null>(null)
const lastError = ref<string | null>(null)
const lastErrorAt = ref(0)
const solution = ref<SolverPlacement[] | null>(null)
const showHint = ref(false)
// epoch (ms) when current level finished loading — used to gate the 30s hint
const loadedAt = ref(0)
// last piece id that was successfully placed — used for the settle animation
const lastPlacedId = ref<number | null>(null)

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
  solution.value = null
  showHint.value = false
  loadedAt.value = Date.now()
  await refresh()
}

async function loadString(text: string): Promise<void> {
  const r = await backend.loadString(text)
  if (!r.ok) {
    lastError.value = r.error ?? 'parse failed'
    lastErrorAt.value = Date.now()
    return
  }
  solution.value = null
  showHint.value = false
  loadedAt.value = Date.now()
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
  // trigger settle animation for this piece's cells
  lastPlacedId.value = pieceId
  window.setTimeout(() => {
    if (lastPlacedId.value === pieceId) lastPlacedId.value = null
  }, 400)
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
  solution.value = null
  showHint.value = false
  loadedAt.value = 0
  lastPlacedId.value = null
}

async function fetchSolution(): Promise<boolean> {
  const r = await backend.solve()
  if (!r.ok) {
    lastError.value = r.error ?? 'no solution'
    lastErrorAt.value = Date.now()
    solution.value = null
    return false
  }
  solution.value = r.solution
  return true
}

async function toggleHint(): Promise<void> {
  if (showHint.value) {
    showHint.value = false
    return
  }
  if (!solution.value) {
    const ok = await fetchSolution()
    if (!ok) return
  }
  showHint.value = true
}

export function useGame(): {
  state: typeof state
  lastError: typeof lastError
  lastErrorAt: typeof lastErrorAt
  solution: typeof solution
  showHint: typeof showHint
  loadedAt: typeof loadedAt
  lastPlacedId: typeof lastPlacedId
  unplacedPieces: ComputedRef<MovablePiece[]>
  placedPieces: ComputedRef<MovablePiece[]>
  load: typeof load
  loadString: typeof loadString
  refresh: typeof refresh
  place: typeof place
  removePiece: typeof removePiece
  reset: typeof reset
  clear: typeof clear
  toggleHint: typeof toggleHint
} {
  const unplacedPieces = computed(() => state.value?.pieces.filter((p) => !p.placed) ?? [])
  const placedPieces = computed(() => state.value?.pieces.filter((p) => p.placed) ?? [])
  return {
    state,
    lastError,
    lastErrorAt,
    solution,
    showHint,
    loadedAt,
    lastPlacedId,
    unplacedPieces,
    placedPieces,
    load,
    loadString,
    refresh,
    place,
    removePiece,
    reset,
    clear,
    toggleHint
  }
}
