<script setup lang="ts">
import { computed, ref } from 'vue'
import CountIndicator from './CountIndicator.vue'
import { useGame } from '../store/game'
import { useDrag, rotateShape, midOf } from '../store/drag'
import { useViewport } from '../store/viewport'

const { state, place } = useGame()
const { drag, start: startDrag, end: endDrag } = useDrag()
const { scale, toStageLocal } = useViewport()

const CELL_SIZE = 64
const GAP = 6

type CellInfo =
  | { kind: 'empty' }
  | { kind: 'blocked' }
  | { kind: 'fixed'; color: number }
  | { kind: 'movable'; pieceId: number; color: number }

const cells = computed<CellInfo[][]>(() => {
  const s = state.value
  if (!s) return []
  const grid: CellInfo[][] = Array.from({ length: s.rows }, () =>
    Array.from({ length: s.cols }, () => ({ kind: 'empty' as const }))
  )
  for (const b of s.blockedCells) grid[b.row][b.col] = { kind: 'blocked' }
  for (const f of s.fixedPieces) grid[f.row][f.col] = { kind: 'fixed', color: f.color }
  for (const p of s.pieces) {
    if (!p.placed) continue
    // hide dragged piece's current cells from the board (it's lifted)
    if (drag.value && drag.value.pieceId === p.id) continue
    const shape = rotateShape(p.baseShape, p.rotation)
    for (let r = 0; r < shape.length; r++) {
      for (let c = 0; c < shape[r].length; c++) {
        if (shape[r][c] !== 1) continue
        const br = p.row + r
        const bc = p.col + c
        if (br >= 0 && br < s.rows && bc >= 0 && bc < s.cols) {
          grid[br][bc] = { kind: 'movable', pieceId: p.id, color: p.color }
        }
      }
    }
  }
  return grid
})

const gridEl = ref<HTMLDivElement | null>(null)

function cellAt(x: number, y: number): { row: number; col: number } | null {
  if (!gridEl.value) return null
  // getBoundingClientRect returns post-scale (visual) coords. Divide by scale
  // to bring mouse coords into design-space (where CELL_SIZE / GAP live).
  const rect = gridEl.value.getBoundingClientRect()
  const s = scale.value || 1
  const padding = 6 // matches .grid padding
  const localX = (x - rect.left) / s - padding
  const localY = (y - rect.top) / s - padding
  if (localX < 0 || localY < 0) return null
  const stride = CELL_SIZE + GAP
  const col = Math.floor(localX / stride)
  const row = Math.floor(localY / stride)
  if (!state.value) return null
  if (row < 0 || row >= state.value.rows) return null
  if (col < 0 || col >= state.value.cols) return null
  // ignore the gap region
  const insideX = localX - col * stride
  const insideY = localY - row * stride
  if (insideX > CELL_SIZE || insideY > CELL_SIZE) return null
  return { row, col }
}

function onCellMouseDown(e: MouseEvent, info: CellInfo): void {
  if (info.kind !== 'movable') return
  if (!state.value) return
  const piece = state.value.pieces.find((p) => p.id === info.pieceId)
  if (!piece) return
  const p = toStageLocal(e.clientX, e.clientY)
  startDrag(piece, p.x, p.y)
}

async function onGridMouseUp(e: MouseEvent): Promise<void> {
  const d = drag.value
  if (!d) return
  const hit = cellAt(e.clientX, e.clientY)
  endDrag()
  if (!hit) return // drop outside the board — nothing happens
  // compute top-left of rotated shape from mid cell
  const shape = rotateShape(d.baseShape, d.rotation)
  const m = midOf(shape)
  const topRow = hit.row - m.row
  const topCol = hit.col - m.col
  await place(d.pieceId, topRow, topCol, d.rotation)
}

// Preview: where the piece WOULD land if dropped right now
const hoverCell = ref<{ row: number; col: number } | null>(null)

function onGridMouseMove(e: MouseEvent): void {
  if (!drag.value) {
    hoverCell.value = null
    return
  }
  hoverCell.value = cellAt(e.clientX, e.clientY)
}

function onGridMouseLeave(): void {
  hoverCell.value = null
}

interface PreviewCellSet {
  cells: Set<number>
  ok: boolean
}

const previewCells = computed<PreviewCellSet | null>(() => {
  const d = drag.value
  const h = hoverCell.value
  const s = state.value
  if (!d || !h || !s) return null
  const shape = rotateShape(d.baseShape, d.rotation)
  const m = midOf(shape)
  const topRow = h.row - m.row
  const topCol = h.col - m.col
  const result = new Set<number>()
  let ok = true
  for (let r = 0; r < shape.length; r++) {
    for (let c = 0; c < shape[r].length; c++) {
      if (shape[r][c] !== 1) continue
      const br = topRow + r
      const bc = topCol + c
      if (br < 0 || br >= s.rows || bc < 0 || bc >= s.cols) {
        ok = false
        continue
      }
      result.add(br * s.cols + bc)
      const grid = cells.value[br][bc]
      if (grid.kind === 'blocked' || grid.kind === 'fixed' || grid.kind === 'movable') {
        ok = false
      }
    }
  }
  return { cells: result, ok }
})

function isPreviewCell(r: number, c: number): boolean {
  if (!previewCells.value || !state.value) return false
  return previewCells.value.cells.has(r * state.value.cols + c)
}
</script>

<template>
  <div
    v-if="state"
    class="game-board"
    :style="{ '--rows': state.rows, '--cols': state.cols }"
  >
    <div class="corner" />

    <div class="col-headers">
      <div v-for="c in state.cols" :key="`col-${c - 1}`" class="header-cell col">
        <CountIndicator
          v-for="color in state.colorCount"
          :key="`col-${c - 1}-${color - 1}`"
          :count="state.colCounts[color - 1][c - 1]"
          :filled="state.colFilled[color - 1][c - 1]"
          :color="color - 1"
          orientation="horizontal"
        />
      </div>
    </div>

    <div class="row-headers">
      <div v-for="r in state.rows" :key="`row-${r - 1}`" class="header-cell row">
        <CountIndicator
          v-for="color in state.colorCount"
          :key="`row-${r - 1}-${color - 1}`"
          :count="state.rowCounts[color - 1][r - 1]"
          :filled="state.rowFilled[color - 1][r - 1]"
          :color="color - 1"
          orientation="vertical"
        />
      </div>
    </div>

    <div
      ref="gridEl"
      class="grid"
      @mousemove="onGridMouseMove"
      @mouseleave="onGridMouseLeave"
      @mouseup="onGridMouseUp"
    >
      <template v-for="(row, r) in cells" :key="`r-${r}`">
        <div
          v-for="(info, c) in row"
          :key="`cell-${r}-${c}`"
          :class="[
            'cell',
            info.kind,
            {
              preview: isPreviewCell(r, c),
              'preview-bad': isPreviewCell(r, c) && previewCells && !previewCells.ok
            }
          ]"
          :data-color="info.kind === 'fixed' || info.kind === 'movable' ? info.color : undefined"
          @mousedown="onCellMouseDown($event, info)"
        >
          <svg v-if="info.kind === 'empty'" class="cell-marks" viewBox="0 0 64 64" preserveAspectRatio="xMidYMid meet">
            <path d="M4 12 L4 4 L12 4" />
            <path d="M52 4 L60 4 L60 12" />
            <path d="M60 52 L60 60 L52 60" />
            <path d="M12 60 L4 60 L4 52" />
            <path d="M26 32 L38 32 M32 26 L32 38" />
          </svg>
          <svg v-if="info.kind === 'blocked'" class="cell-marks blocked-mark" viewBox="0 0 64 64">
            <circle cx="32" cy="32" r="14" fill="none" />
            <path d="M22 22 L42 42" />
          </svg>
        </div>
      </template>
    </div>
  </div>
</template>

<style scoped>
.game-board {
  --cell-size: 64px;
  --header-size: 56px;
  --gap: 6px;
  display: grid;
  grid-template-columns: var(--header-size) 1fr;
  grid-template-rows: var(--header-size) 1fr;
  gap: var(--gap);
}

.corner {
  grid-column: 1;
  grid-row: 1;
}

.col-headers {
  grid-column: 2;
  grid-row: 1;
  display: grid;
  grid-template-columns: repeat(var(--cols), var(--cell-size));
  gap: var(--gap);
}

.row-headers {
  grid-column: 1;
  grid-row: 2;
  display: grid;
  grid-template-rows: repeat(var(--rows), var(--cell-size));
  gap: var(--gap);
}

.header-cell {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 4px;
}
.header-cell.col {
  flex-direction: row;
}
.header-cell.row {
  flex-direction: column;
}

.grid {
  grid-column: 2;
  grid-row: 2;
  display: grid;
  grid-template-columns: repeat(var(--cols), var(--cell-size));
  grid-template-rows: repeat(var(--rows), var(--cell-size));
  gap: var(--gap);
  padding: 6px;
  border-radius: 4px;
  background:
    radial-gradient(ellipse at center, rgba(8, 12, 18, 0.65), rgba(4, 6, 10, 0.85));
  box-shadow:
    inset 0 1px 4px rgba(0, 0, 0, 0.7),
    0 0 0 1px rgba(255, 255, 255, 0.04);
}

.cell {
  position: relative;
  background: #161a22;
  border-top: 1px solid rgba(255, 255, 255, 0.025);
  border-bottom: 1px solid rgba(0, 0, 0, 0.5);
  overflow: hidden;
}

.cell.movable {
  cursor: grab;
}

.cell-marks {
  position: absolute;
  inset: 0;
  width: 100%;
  height: 100%;
  fill: none;
  stroke: rgba(180, 200, 220, 0.18);
  stroke-width: 1.2;
  stroke-linecap: square;
  pointer-events: none;
}

.cell.blocked {
  background:
    repeating-linear-gradient(
      45deg,
      rgba(255, 255, 255, 0.04) 0 2px,
      transparent 2px 6px
    ),
    #1d1f25;
  border-top-color: rgba(255, 255, 255, 0.03);
  border-bottom-color: rgba(0, 0, 0, 0.5);
}
.cell .blocked-mark {
  stroke: rgba(220, 220, 220, 0.35);
  stroke-width: 2;
}

.cell.fixed,
.cell.movable {
  border-top: none;
  border-bottom: none;
}
.cell.fixed[data-color='0'],
.cell.movable[data-color='0'] {
  background: linear-gradient(180deg, #98c52a 0%, #7ba620 50%, #6a8e1c 100%);
  box-shadow:
    inset 0 0 0 1px #c1ec3e,
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
}
.cell.fixed[data-color='1'],
.cell.movable[data-color='1'] {
  background: linear-gradient(180deg, #4ca8c4 0%, #347a92 50%, #275f76 100%);
  box-shadow:
    inset 0 0 0 1px #7ed0e6,
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
}

/* drop preview */
.cell.preview {
  outline: 2px dashed rgba(184, 232, 53, 0.7);
  outline-offset: -2px;
}
.cell.preview-bad {
  outline-color: rgba(232, 80, 80, 0.7);
}
</style>
