<script setup lang="ts">
import { reactive, ref, computed } from 'vue'

const emit = defineEmits<{
  back: []
  play: [config: string]
}>()

type CellKind = 'empty' | 'blocked' | 'fixed'
interface CellState {
  kind: CellKind
  color: number // valid for 'fixed'
}
type Tool = { kind: 'empty' } | { kind: 'blocked' } | { kind: 'fixed'; color: number }

interface PieceEdit {
  color: number
  shape: number[][]
}

interface Designer {
  rows: number
  cols: number
  colorCount: number
  cells: CellState[][]
  rowCounts: number[][] // [color][row]
  colCounts: number[][] // [color][col]
  pieces: PieceEdit[]
}

function emptyCell(): CellState {
  return { kind: 'empty', color: -1 }
}

function makeGrid(rows: number, cols: number, fill: () => CellState): CellState[][] {
  return Array.from({ length: rows }, () => Array.from({ length: cols }, fill))
}

function makeCounts(colors: number, len: number): number[][] {
  return Array.from({ length: colors }, () => Array.from({ length: len }, () => 0))
}

const initialRows = 5
const initialCols = 5

const d: Designer = reactive({
  rows: initialRows,
  cols: initialCols,
  colorCount: 1,
  cells: makeGrid(initialRows, initialCols, emptyCell),
  rowCounts: makeCounts(1, initialRows),
  colCounts: makeCounts(1, initialCols),
  pieces: [{ color: 0, shape: [[1]] }]
})

const tool = ref<Tool>({ kind: 'blocked' })
const selectedPiece = ref<number>(0)

function resizeBoard(rows: number, cols: number): void {
  const newCells = makeGrid(rows, cols, emptyCell)
  for (let r = 0; r < Math.min(rows, d.rows); r++)
    for (let c = 0; c < Math.min(cols, d.cols); c++) newCells[r][c] = d.cells[r][c]
  d.cells = newCells
  // Resize per-color row/col targets, preserving existing values where they fit.
  const newRow: number[][] = []
  const newCol: number[][] = []
  for (let i = 0; i < d.colorCount; i++) {
    const oldR = d.rowCounts[i] ?? []
    const oldC = d.colCounts[i] ?? []
    newRow.push(Array.from({ length: rows }, (_, r) => oldR[r] ?? 0))
    newCol.push(Array.from({ length: cols }, (_, c) => oldC[c] ?? 0))
  }
  d.rowCounts = newRow
  d.colCounts = newCol
  d.rows = rows
  d.cols = cols
}

function setColorCount(n: number): void {
  d.colorCount = n
  // Drop fixed cells of removed colors
  for (let r = 0; r < d.rows; r++)
    for (let c = 0; c < d.cols; c++) {
      const v = d.cells[r][c]
      if (v.kind === 'fixed' && v.color >= n) d.cells[r][c] = emptyCell()
    }
  // Drop pieces of removed colors
  d.pieces = d.pieces.filter((p) => p.color < n)
  // Trim/extend row/col target arrays per color
  const newRow: number[][] = []
  const newCol: number[][] = []
  for (let i = 0; i < n; i++) {
    newRow.push(d.rowCounts[i] ?? Array.from({ length: d.rows }, () => 0))
    newCol.push(d.colCounts[i] ?? Array.from({ length: d.cols }, () => 0))
  }
  d.rowCounts = newRow
  d.colCounts = newCol
  // Tool may reference a now-invalid color
  if (tool.value.kind === 'fixed' && tool.value.color >= n) tool.value = { kind: 'empty' }
}

function onCellClick(r: number, c: number, ev: MouseEvent): void {
  // Right-click clears the cell
  if (ev.button === 2) {
    d.cells[r][c] = emptyCell()
    return
  }
  switch (tool.value.kind) {
    case 'empty':
      d.cells[r][c] = emptyCell()
      break
    case 'blocked':
      d.cells[r][c] = { kind: 'blocked', color: -1 }
      break
    case 'fixed':
      d.cells[r][c] = { kind: 'fixed', color: tool.value.color }
      break
  }
}

function addPiece(): void {
  d.pieces.push({ color: 0, shape: [[1]] })
  selectedPiece.value = d.pieces.length - 1
}

function removePiece(idx: number): void {
  d.pieces.splice(idx, 1)
  if (selectedPiece.value >= d.pieces.length) selectedPiece.value = d.pieces.length - 1
}

function resizePiece(idx: number, rows: number, cols: number): void {
  const p = d.pieces[idx]
  const next = Array.from({ length: rows }, () => Array.from({ length: cols }, () => 0))
  for (let r = 0; r < Math.min(rows, p.shape.length); r++)
    for (let c = 0; c < Math.min(cols, p.shape[r]?.length ?? 0); c++)
      next[r][c] = p.shape[r][c]
  p.shape = next
}

function togglePieceCell(idx: number, r: number, c: number): void {
  const p = d.pieces[idx]
  p.shape[r][c] = p.shape[r][c] === 1 ? 0 : 1
}

const selected = computed(() => d.pieces[selectedPiece.value])

// Build the config-file format string per the PDF spec.
function buildConfig(): string {
  const lines: string[] = []
  lines.push(`${d.colorCount} ${d.rows} ${d.cols}`)
  for (let color = 0; color < d.colorCount; color++) {
    lines.push(d.rowCounts[color].join(' '))
    lines.push(d.colCounts[color].join(' '))
    const fixed: [number, number][] = []
    for (let r = 0; r < d.rows; r++)
      for (let c = 0; c < d.cols; c++) {
        const v = d.cells[r][c]
        if (v.kind === 'fixed' && v.color === color) fixed.push([r, c])
      }
    lines.push(String(fixed.length))
    for (const [r, c] of fixed) lines.push(`${r} ${c}`)
  }
  const blocked: [number, number][] = []
  for (let r = 0; r < d.rows; r++)
    for (let c = 0; c < d.cols; c++) {
      if (d.cells[r][c].kind === 'blocked') blocked.push([r, c])
    }
  lines.push(String(blocked.length))
  for (const [r, c] of blocked) lines.push(`${r} ${c}`)
  for (const p of d.pieces) {
    const rows = p.shape.length
    const cols = p.shape[0]?.length ?? 0
    lines.push(`${p.color} ${rows} ${cols}`)
    for (const row of p.shape) lines.push(row.join(' '))
  }
  return lines.join('\n') + '\n'
}

const showExport = ref(false)
const exportText = computed(() => buildConfig())

async function onSave(): Promise<void> {
  const path = await window.api.dialog.saveLevel(exportText.value)
  if (path) {
    saveResult.value = `已存到 ${path}`
    setTimeout(() => (saveResult.value = ''), 3000)
  }
}

const saveResult = ref('')

function onPlay(): void {
  emit('play', buildConfig())
}

const cellColor = (k: CellState): string | undefined => {
  if (k.kind === 'fixed') return String(k.color)
  return undefined
}
</script>

<template>
  <div class="designer-root">
    <div class="header">
      <button class="bar-btn" @click="emit('back')">← 返回</button>
      <span class="title">關卡設計</span>
      <span style="flex: 1" />
      <button class="bar-btn" @click="showExport = true">輸出文字</button>
      <button class="bar-btn" @click="onSave">存檔</button>
      <button class="bar-btn primary" @click="onPlay">遊玩</button>
    </div>

    <div class="body">
      <!-- Left: board editor -->
      <section class="board-side">
        <div class="row-controls">
          <label
            >Row
            <input
              type="number"
              min="1"
              max="12"
              :value="d.rows"
              @change="resizeBoard(Number(($event.target as HTMLInputElement).value), d.cols)"
          /></label>
          <label
            >Col
            <input
              type="number"
              min="1"
              max="12"
              :value="d.cols"
              @change="resizeBoard(d.rows, Number(($event.target as HTMLInputElement).value))"
          /></label>
          <label
            >Colors
            <select
              :value="d.colorCount"
              @change="setColorCount(Number(($event.target as HTMLSelectElement).value))"
            >
              <option :value="1">1</option>
              <option :value="2">2</option>
            </select>
          </label>
        </div>

        <div class="tool-row">
          <span class="tool-label">工具:</span>
          <button
            class="tool"
            :class="{ active: tool.kind === 'empty' }"
            @click="tool = { kind: 'empty' }"
          >
            空格
          </button>
          <button
            class="tool"
            :class="{ active: tool.kind === 'blocked' }"
            @click="tool = { kind: 'blocked' }"
          >
            禁區
          </button>
          <button
            v-for="ci in d.colorCount"
            :key="`fix-${ci - 1}`"
            class="tool"
            :class="{
              active: tool.kind === 'fixed' && tool.color === ci - 1
            }"
            :data-color="ci - 1"
            @click="tool = { kind: 'fixed', color: ci - 1 }"
          >
            固定 {{ ci - 1 }}
          </button>
        </div>

        <div
          class="board-wrap"
          :style="{ '--rows': d.rows, '--cols': d.cols }"
        >
          <!-- corner -->
          <div class="corner" />
          <!-- column count inputs (per color, stacked) -->
          <div class="col-counts">
            <div v-for="c in d.cols" :key="`col-${c - 1}`" class="col-stack">
              <input
                v-for="color in d.colorCount"
                :key="`col-${c - 1}-${color - 1}`"
                type="number"
                min="0"
                class="count-input"
                :data-color="color - 1"
                :value="d.colCounts[color - 1][c - 1]"
                @change="
                  d.colCounts[color - 1][c - 1] = Number(($event.target as HTMLInputElement).value)
                "
              />
            </div>
          </div>
          <!-- row count inputs -->
          <div class="row-counts">
            <div v-for="r in d.rows" :key="`row-${r - 1}`" class="row-stack">
              <input
                v-for="color in d.colorCount"
                :key="`row-${r - 1}-${color - 1}`"
                type="number"
                min="0"
                class="count-input"
                :data-color="color - 1"
                :value="d.rowCounts[color - 1][r - 1]"
                @change="
                  d.rowCounts[color - 1][r - 1] = Number(($event.target as HTMLInputElement).value)
                "
              />
            </div>
          </div>
          <!-- board -->
          <div class="board" @contextmenu.prevent>
            <template v-for="(row, r) in d.cells" :key="`r-${r}`">
              <div
                v-for="(cell, c) in row"
                :key="`b-${r}-${c}`"
                :class="['dcell', cell.kind]"
                :data-color="cellColor(cell)"
                @mousedown="onCellClick(r, c, $event)"
                @contextmenu.prevent
              />
            </template>
          </div>
        </div>
      </section>

      <!-- Right: pieces -->
      <section class="pieces-side">
        <div class="piece-header">
          <span>零件</span>
          <button class="bar-btn" @click="addPiece">+ 新增</button>
        </div>
        <div class="piece-list">
          <button
            v-for="(p, i) in d.pieces"
            :key="i"
            class="piece-thumb"
            :class="{ active: selectedPiece === i }"
            @click="selectedPiece = i"
          >
            <span class="thumb-id">#{{ i }}</span>
            <div class="thumb-shape" :data-color="p.color">
              <div v-for="(row, r) in p.shape" :key="r" class="thumb-row">
                <div
                  v-for="(v, c) in row"
                  :key="c"
                  :class="['thumb-cell', { on: v === 1 }]"
                />
              </div>
            </div>
          </button>
        </div>

        <div v-if="selected" class="piece-editor">
          <div class="piece-edit-row">
            <label
              >Rows
              <input
                type="number"
                min="1"
                max="8"
                :value="selected.shape.length"
                @change="
                  resizePiece(
                    selectedPiece,
                    Number(($event.target as HTMLInputElement).value),
                    selected.shape[0]?.length ?? 1
                  )
                "
            /></label>
            <label
              >Cols
              <input
                type="number"
                min="1"
                max="8"
                :value="selected.shape[0]?.length ?? 0"
                @change="
                  resizePiece(
                    selectedPiece,
                    selected.shape.length,
                    Number(($event.target as HTMLInputElement).value)
                  )
                "
            /></label>
            <label
              >Color
              <select
                :value="selected.color"
                @change="selected.color = Number(($event.target as HTMLSelectElement).value)"
              >
                <option v-for="ci in d.colorCount" :key="ci - 1" :value="ci - 1">{{ ci - 1 }}</option>
              </select>
            </label>
            <button class="bar-btn danger" @click="removePiece(selectedPiece)">刪除</button>
          </div>

          <div class="shape-editor" :data-color="selected.color">
            <div v-for="(row, r) in selected.shape" :key="r" class="shape-row">
              <div
                v-for="(v, c) in row"
                :key="c"
                :class="['shape-cell', { on: v === 1 }]"
                @click="togglePieceCell(selectedPiece, r, c)"
              />
            </div>
          </div>
        </div>
      </section>
    </div>

    <transition name="fade">
      <div v-if="saveResult" class="save-toast">{{ saveResult }}</div>
    </transition>

    <div v-if="showExport" class="export-modal" @click.self="showExport = false">
      <div class="export-inner">
        <h3>關卡設定檔內容</h3>
        <textarea readonly :value="exportText" />
        <div class="modal-actions">
          <button class="bar-btn" @click="showExport = false">關閉</button>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.designer-root {
  position: absolute;
  inset: 0;
  display: flex;
  flex-direction: column;
  color: #cbd6e2;
  padding: 16px 24px;
  gap: 12px;
  font-size: 13px;
}

.header {
  display: flex;
  align-items: center;
  gap: 8px;
}
.title {
  font-size: 18px;
  letter-spacing: 4px;
  color: #d8f96a;
}

.body {
  flex: 1;
  display: flex;
  gap: 20px;
  min-height: 0;
}

.bar-btn {
  padding: 6px 14px;
  background: rgba(14, 18, 26, 0.5);
  color: #cbd6e2;
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 4px;
  font-family: inherit;
  font-size: 13px;
  cursor: pointer;
}
.bar-btn:hover {
  background: rgba(30, 40, 60, 0.6);
  border-color: rgba(255, 255, 255, 0.25);
}
.bar-btn.primary {
  background: rgba(184, 232, 53, 0.2);
  border-color: rgba(184, 232, 53, 0.5);
  color: #d8f96a;
}
.bar-btn.danger {
  background: rgba(180, 60, 60, 0.2);
  border-color: rgba(232, 80, 80, 0.5);
}

/* board side */
.board-side {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.row-controls {
  display: flex;
  gap: 14px;
  align-items: center;
}
.row-controls label,
.piece-edit-row label {
  display: flex;
  align-items: center;
  gap: 6px;
}
.row-controls input,
.piece-edit-row input,
.row-controls select,
.piece-edit-row select {
  width: 60px;
  padding: 4px 6px;
  background: rgba(14, 18, 26, 0.5);
  color: #cbd6e2;
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 3px;
  font-family: inherit;
  font-size: 13px;
}

.tool-row {
  display: flex;
  align-items: center;
  gap: 6px;
}
.tool-label {
  margin-right: 4px;
  color: rgba(203, 214, 226, 0.6);
}
.tool {
  padding: 4px 12px;
  background: rgba(14, 18, 26, 0.5);
  border: 1px solid rgba(255, 255, 255, 0.15);
  color: #cbd6e2;
  border-radius: 3px;
  cursor: pointer;
  font-family: inherit;
  font-size: 12px;
}
.tool.active {
  background: rgba(184, 232, 53, 0.2);
  border-color: rgba(184, 232, 53, 0.5);
  color: #d8f96a;
}
.tool[data-color='1'].active {
  background: rgba(78, 192, 224, 0.2);
  border-color: rgba(78, 192, 224, 0.5);
  color: #82dcf2;
}

.board-wrap {
  --cell-size: 36px;
  --hdr-size: 44px;
  --gap: 3px;
  display: grid;
  grid-template-columns: var(--hdr-size) 1fr;
  grid-template-rows: var(--hdr-size) 1fr;
  gap: var(--gap);
}
.corner {
  grid-column: 1;
  grid-row: 1;
}
.col-counts {
  grid-column: 2;
  display: grid;
  grid-template-columns: repeat(var(--cols), var(--cell-size));
  gap: var(--gap);
}
.col-stack {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2px;
}
.row-counts {
  grid-row: 2;
  display: grid;
  grid-template-rows: repeat(var(--rows), var(--cell-size));
  gap: var(--gap);
}
.row-stack {
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: center;
  gap: 2px;
  height: 100%;
}
.count-input {
  width: 22px;
  padding: 1px 2px;
  background: rgba(14, 18, 26, 0.5);
  color: #b8e835;
  border: 1px solid rgba(184, 232, 53, 0.3);
  border-radius: 2px;
  text-align: center;
  font-size: 12px;
  font-family: inherit;
  -moz-appearance: textfield;
}
.count-input::-webkit-outer-spin-button,
.count-input::-webkit-inner-spin-button {
  -webkit-appearance: none;
  margin: 0;
}
.count-input[data-color='1'] {
  color: #4ec0e0;
  border-color: rgba(78, 192, 224, 0.3);
}

.board {
  grid-column: 2;
  grid-row: 2;
  display: grid;
  grid-template-columns: repeat(var(--cols), var(--cell-size));
  grid-template-rows: repeat(var(--rows), var(--cell-size));
  gap: var(--gap);
  padding: 4px;
  border-radius: 4px;
  background: radial-gradient(ellipse at center, rgba(8, 12, 18, 0.7), rgba(4, 6, 10, 0.85));
}
.dcell {
  background: #161a22;
  border-top: 1px solid rgba(255, 255, 255, 0.025);
  border-bottom: 1px solid rgba(0, 0, 0, 0.5);
  cursor: pointer;
}
.dcell.blocked {
  background: repeating-linear-gradient(
      45deg,
      rgba(255, 255, 255, 0.04) 0 2px,
      transparent 2px 6px
    ),
    #1d1f25;
}
.dcell.fixed[data-color='0'] {
  background: linear-gradient(180deg, #98c52a 0%, #6a8e1c 100%);
  box-shadow: inset 0 0 0 1px #c1ec3e;
}
.dcell.fixed[data-color='1'] {
  background: linear-gradient(180deg, #4ca8c4 0%, #275f76 100%);
  box-shadow: inset 0 0 0 1px #7ed0e6;
}

/* pieces side */
.pieces-side {
  width: 380px;
  display: flex;
  flex-direction: column;
  gap: 10px;
  padding: 12px;
  background: rgba(8, 12, 18, 0.4);
  border: 1px solid rgba(255, 255, 255, 0.05);
  border-radius: 6px;
  overflow-y: auto;
}
.piece-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 14px;
  color: rgba(203, 214, 226, 0.7);
}
.piece-list {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(80px, 1fr));
  gap: 8px;
}
.piece-thumb {
  position: relative;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 14px 6px 6px;
  background: rgba(14, 18, 26, 0.5);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 4px;
  cursor: pointer;
  min-height: 64px;
}
.piece-thumb.active {
  border-color: rgba(184, 232, 53, 0.7);
  background: rgba(30, 40, 30, 0.6);
}
.thumb-id {
  position: absolute;
  top: 2px;
  left: 4px;
  font-size: 10px;
  color: rgba(203, 214, 226, 0.5);
}
.thumb-shape {
  display: flex;
  flex-direction: column;
  gap: 1px;
}
.thumb-row {
  display: flex;
  gap: 1px;
}
.thumb-cell {
  width: 10px;
  height: 10px;
}
.thumb-cell.on {
  background: #b8e835;
}
.thumb-shape[data-color='1'] .thumb-cell.on {
  background: #4ec0e0;
}

.piece-editor {
  display: flex;
  flex-direction: column;
  gap: 10px;
  padding: 10px;
  background: rgba(14, 18, 26, 0.4);
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 4px;
}
.piece-edit-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  align-items: center;
}
.shape-editor {
  display: flex;
  flex-direction: column;
  gap: 2px;
  align-items: flex-start;
}
.shape-row {
  display: flex;
  gap: 2px;
}
.shape-cell {
  width: 26px;
  height: 26px;
  background: rgba(40, 50, 60, 0.4);
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 2px;
  cursor: pointer;
}
.shape-cell.on {
  background: #98c52a;
  border-color: #c1ec3e;
}
.shape-editor[data-color='1'] .shape-cell.on {
  background: #4ca8c4;
  border-color: #7ed0e6;
}

.save-toast {
  position: absolute;
  bottom: 24px;
  left: 50%;
  transform: translateX(-50%);
  padding: 8px 16px;
  background: rgba(40, 60, 40, 0.85);
  border: 1px solid rgba(184, 232, 53, 0.4);
  border-radius: 4px;
  font-size: 13px;
  pointer-events: none;
}

.export-modal {
  position: absolute;
  inset: 0;
  background: rgba(0, 0, 0, 0.6);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 20;
}
.export-inner {
  width: 600px;
  max-height: 80%;
  display: flex;
  flex-direction: column;
  gap: 10px;
  padding: 20px;
  background: rgba(14, 18, 26, 0.95);
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 6px;
}
.export-inner h3 {
  margin: 0;
  font-size: 16px;
  letter-spacing: 2px;
}
.export-inner textarea {
  flex: 1;
  min-height: 260px;
  background: #0a0e14;
  color: #cbd6e2;
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 4px;
  padding: 10px;
  font-family: 'JetBrains Mono', 'Consolas', monospace;
  font-size: 12px;
  white-space: pre;
  resize: none;
}
.modal-actions {
  display: flex;
  justify-content: flex-end;
}

.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s;
}
.fade-enter-from,
.fade-leave-to {
  opacity: 0;
}
</style>
