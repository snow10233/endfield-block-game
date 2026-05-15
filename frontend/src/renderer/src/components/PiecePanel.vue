<script setup lang="ts">
import { computed } from 'vue'
import PieceView from './PieceView.vue'
import { useGame } from '../store/game'
import { useDrag } from '../store/drag'
import { useViewport } from '../store/viewport'
import type { MovablePiece } from '../types/level'

const { unplacedPieces, state, removePiece } = useGame()
const { drag, start: startDrag, end: endDrag } = useDrag()
const { toStageLocal } = useViewport()

function onPieceMouseDown(e: MouseEvent, piece: MovablePiece): void {
  e.preventDefault()
  const p = toStageLocal(e.clientX, e.clientY)
  startDrag(piece, p.x, p.y)
}

// If a piece that's currently on the board is dropped onto the panel, remove
// it from the board (i.e., return it to inventory).
async function onPanelMouseUp(): Promise<void> {
  const d = drag.value
  if (!d || !state.value) return
  const piece = state.value.pieces.find((p) => p.id === d.pieceId)
  if (!piece) return
  // Always end the drag so the window-level fallback doesn't fire too.
  endDrag()
  if (piece.placed) {
    await removePiece(d.pieceId)
  }
}

// Visual cue: when dragging a piece that's currently placed, mark the panel
// as a valid drop target.
const dropTargetActive = computed(() => {
  const d = drag.value
  if (!d || !state.value) return false
  const piece = state.value.pieces.find((p) => p.id === d.pieceId)
  return !!piece?.placed
})
</script>

<template>
  <div
    :class="['piece-panel', { 'drop-target': dropTargetActive }]"
    @mouseup="onPanelMouseUp"
  >
    <div
      v-for="piece in unplacedPieces"
      :key="piece.id"
      class="slot"
      @mousedown="onPieceMouseDown($event, piece)"
    >
      <span class="slot-id">{{ piece.id }}</span>
      <PieceView :piece="piece" />
    </div>
  </div>
</template>

<style scoped>
.piece-panel {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 10px;
  padding: 10px;
  min-height: 220px;
  min-width: 240px;
  align-content: start;
  border-radius: 8px;
  background:
    radial-gradient(ellipse at center, rgba(8, 12, 18, 0.5), rgba(4, 6, 10, 0.7));
  box-shadow:
    inset 0 2px 8px rgba(0, 0, 0, 0.5),
    0 0 0 1px rgba(255, 255, 255, 0.03);
  transition: box-shadow 0.15s ease;
}
.piece-panel.drop-target {
  box-shadow:
    inset 0 0 0 2px rgba(184, 232, 53, 0.6),
    inset 0 2px 8px rgba(0, 0, 0, 0.5);
}
.slot {
  position: relative;
  display: flex;
  align-items: center;
  justify-content: center;
  min-width: 110px;
  min-height: 110px;
  padding: 14px;
  border-radius: 4px;
  background:
    linear-gradient(155deg, rgba(40, 50, 64, 0.35) 0%, rgba(18, 22, 30, 0.55) 100%);
  box-shadow:
    inset 0 1px 0 rgba(255, 255, 255, 0.04),
    inset 0 -1px 0 rgba(0, 0, 0, 0.4);
  overflow: hidden;
  cursor: grab;
  user-select: none;
}
.slot:active {
  cursor: grabbing;
}
.slot::before {
  content: '';
  position: absolute;
  inset: 6px;
  pointer-events: none;
  border: 1px solid rgba(180, 200, 220, 0.08);
  mask:
    linear-gradient(to right, black 10px, transparent 10px, transparent calc(100% - 10px), black calc(100% - 10px)) top / 100% 1px no-repeat,
    linear-gradient(to right, black 10px, transparent 10px, transparent calc(100% - 10px), black calc(100% - 10px)) bottom / 100% 1px no-repeat,
    linear-gradient(to bottom, black 10px, transparent 10px, transparent calc(100% - 10px), black calc(100% - 10px)) left / 1px 100% no-repeat,
    linear-gradient(to bottom, black 10px, transparent 10px, transparent calc(100% - 10px), black calc(100% - 10px)) right / 1px 100% no-repeat;
}
.slot-id {
  position: absolute;
  top: 6px;
  left: 8px;
  font-size: 11px;
  font-weight: 600;
  letter-spacing: 0.5px;
  color: #d44a55;
  font-family: 'JetBrains Mono', 'Consolas', monospace;
  z-index: 1;
}
</style>
