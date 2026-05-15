<script setup lang="ts">
import { onMounted, onUnmounted, computed } from 'vue'
import { useDrag, rotateShape, midOf } from '../store/drag'
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

const rotatedShape = computed(() => {
  if (!drag.value) return null
  return rotateShape(drag.value.baseShape, drag.value.rotation)
})

const mid = computed(() => {
  const s = rotatedShape.value
  if (!s) return { row: 0, col: 0 }
  return midOf(s)
})

// drag.x / drag.y are now in design-space (stage-local) coords.
// Preview is `position: absolute` inside the stage, so its left/top are
// design-space too — and the visual scale comes from the stage's transform.
const previewStyle = computed(() => {
  if (!drag.value) return {}
  const offsetX = drag.value.x - mid.value.col * (CELL_SIZE + CELL_GAP) - CELL_SIZE / 2
  const offsetY = drag.value.y - mid.value.row * (CELL_SIZE + CELL_GAP) - CELL_SIZE / 2
  return {
    left: `${offsetX}px`,
    top: `${offsetY}px`
  }
})
</script>

<template>
  <div
    v-if="drag && rotatedShape"
    class="drag-preview"
    :style="previewStyle"
    :data-color="drag.color"
  >
    <div v-for="(row, r) in rotatedShape" :key="r" class="row">
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
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
  opacity: 0.9;
}
.drag-preview[data-color='0'] {
  --top: #98c52a;
  --mid: #7ba620;
  --bot: #6a8e1c;
  --stroke: #c1ec3e;
}
.drag-preview[data-color='1'] {
  --top: #4ca8c4;
  --mid: #347a92;
  --bot: #275f76;
  --stroke: #7ed0e6;
}
</style>
