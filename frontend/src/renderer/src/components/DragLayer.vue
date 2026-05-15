<script setup lang="ts">
import { onMounted, onUnmounted, computed } from 'vue'
import { useDrag, midOf } from '../store/drag'
import { useGame } from '../store/game'
import { useViewport } from '../store/viewport'

const { drag, move, rotate, end, cancel } = useDrag()
const { state, removePiece } = useGame()
const { toStageLocal } = useViewport()

function onMouseMove(e: MouseEvent): void {
  if (!drag.value) return
  const p = toStageLocal(e.clientX, e.clientY)
  move(p.x, p.y)
}

function onMouseUp(): void {
  if (!drag.value) return
  cancel()
}

async function onKeyDown(e: KeyboardEvent): Promise<void> {
  if (!drag.value) return
  if (e.key === 'r' || e.key === 'R') {
    rotate()
    e.preventDefault()
  } else if (e.key === 'Escape') {
    const d = end()
    e.preventDefault()
    if (d && state.value) {
      const piece = state.value.pieces.find((p) => p.id === d.pieceId)
      if (piece && piece.placed) {
        await removePiece(d.pieceId)
      }
    }
  }
}

onMounted(() => {
  window.addEventListener('mousemove', onMouseMove)
  window.addEventListener('mouseup', onMouseUp)
  window.addEventListener('keydown', onKeyDown)
})

onUnmounted(() => {
  window.removeEventListener('mousemove', onMouseMove)
  window.removeEventListener('mouseup', onMouseUp)
  window.removeEventListener('keydown', onKeyDown)
})

const CELL_SIZE = 28
const CELL_GAP = 2
const STRIDE = CELL_SIZE + CELL_GAP

// Mid of the base (un-rotated) shape — stays constant for the duration of a
// drag. CSS rotation pivots around this cell so visual rotation feels
// "anchored" to where the cursor is.
const baseMid = computed(() => {
  if (!drag.value) return { row: 0, col: 0 }
  return midOf(drag.value.baseShape)
})

const previewStyle = computed(() => {
  if (!drag.value) return {}
  const m = baseMid.value
  const leftPx = drag.value.x - m.col * STRIDE - CELL_SIZE / 2
  const topPx = drag.value.y - m.row * STRIDE - CELL_SIZE / 2
  const originX = m.col * STRIDE + CELL_SIZE / 2
  const originY = m.row * STRIDE + CELL_SIZE / 2
  return {
    left: `${leftPx}px`,
    top: `${topPx}px`,
    transform: `rotate(${drag.value.rotationCount * 90}deg)`,
    transformOrigin: `${originX}px ${originY}px`
  }
})
</script>

<template>
  <div
    v-if="drag"
    class="drag-preview"
    :style="previewStyle"
    :data-color="drag.color"
  >
    <div v-for="(row, r) in drag.baseShape" :key="r" class="row">
      <div
        v-for="(v, c) in row"
        :key="c"
        :class="['drag-cell', { filled: v === 1 }]"
      />
    </div>
  </div>
</template>

<style scoped>
.drag-preview {
  position: absolute;
  z-index: 100;
  pointer-events: none;
  display: flex;
  flex-direction: column;
  gap: 2px;
  /* transform changes (rotation) animate; left/top jump instantly per frame */
  transition: transform 180ms cubic-bezier(0.34, 1.56, 0.64, 1);
  /* slight brightness boost so the dragged piece pops vs placed ones */
  filter: brightness(1.1) saturate(1.15);
}
.row {
  display: flex;
  gap: 2px;
}
.drag-cell {
  width: 28px;
  height: 28px;
}
.drag-cell.filled {
  background: linear-gradient(180deg, var(--top, #98c52a) 0%, var(--mid, #7ba620) 50%, var(--bot, #6a8e1c) 100%);
  box-shadow:
    inset 0 0 0 1px var(--stroke, #c1ec3e),
    inset 0 2px 0 rgba(255, 255, 255, 0.2),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
  opacity: 0.95;
}
.drag-preview[data-color='0'] {
  --top: #b8e835;
  --mid: #98c52a;
  --bot: #7ba620;
  --stroke: #d8f96a;
}
.drag-preview[data-color='1'] {
  --top: #6cd0e8;
  --mid: #4ca8c4;
  --bot: #347a92;
  --stroke: #a0e0f0;
}
</style>
