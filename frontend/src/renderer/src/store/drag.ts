import { ref, computed, type ComputedRef } from 'vue'
import type { MovablePiece } from '../types/level'

export function rotateShape(shape: number[][], rot: number): number[][] {
  let s = shape
  const n = ((rot % 4) + 4) % 4
  for (let k = 0; k < n; k++) {
    const r = s.length
    const c = s[0]?.length ?? 0
    const next: number[][] = Array.from({ length: c }, () => Array(r).fill(0))
    for (let i = 0; i < r; i++) for (let j = 0; j < c; j++) next[j][r - 1 - i] = s[i][j]
    s = next
  }
  return s
}

export function midOf(shape: number[][]): { row: number; col: number } {
  for (let r = 0; r < shape.length; r++) {
    for (let c = 0; c < shape[r].length; c++) {
      if (shape[r][c] === 1) return { row: r, col: c }
    }
  }
  return { row: 0, col: 0 }
}

export interface DragState {
  pieceId: number
  color: number
  baseShape: number[][]
  rotation: number
  // mouse position in viewport coords
  x: number
  y: number
}

const drag = ref<DragState | null>(null)

export function useDrag(): {
  drag: typeof drag
  isDragging: ComputedRef<boolean>
  start: (piece: MovablePiece, x: number, y: number) => void
  move: (x: number, y: number) => void
  rotate: () => void
  end: () => DragState | null
  cancel: () => void
} {
  function start(piece: MovablePiece, x: number, y: number): void {
    drag.value = {
      pieceId: piece.id,
      color: piece.color,
      baseShape: piece.baseShape,
      rotation: piece.rotation,
      x,
      y
    }
  }
  function move(x: number, y: number): void {
    if (!drag.value) return
    drag.value.x = x
    drag.value.y = y
  }
  function rotate(): void {
    if (!drag.value) return
    drag.value.rotation = (drag.value.rotation + 1) % 4
  }
  function end(): DragState | null {
    const v = drag.value
    drag.value = null
    return v
  }
  function cancel(): void {
    drag.value = null
  }
  const isDragging = computed<boolean>(() => drag.value !== null)
  return { drag, isDragging, start, move, rotate, end, cancel }
}
