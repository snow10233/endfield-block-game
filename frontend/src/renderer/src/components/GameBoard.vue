<script setup lang="ts">
import type { Level } from '../types/level'
import CountIndicator from './CountIndicator.vue'

const props = defineProps<{ level: Level }>()

function isBlocked(r: number, c: number): boolean {
  return props.level.blockedCells.some((b) => b.row === r && b.col === c)
}

function fixedAt(r: number, c: number): number | null {
  const f = props.level.fixedPieces.find((p) => p.row === r && p.col === c)
  return f ? f.color : null
}
</script>

<template>
  <div
    class="game-board"
    :style="{ '--rows': level.rows, '--cols': level.cols }"
  >
    <div class="corner" />

    <div class="col-headers">
      <div v-for="c in level.cols" :key="`col-${c - 1}`" class="header-cell col">
        <CountIndicator
          v-for="color in level.colorCount"
          :key="`col-${c - 1}-${color - 1}`"
          :count="level.colCounts[color - 1][c - 1]"
          :color="color - 1"
          orientation="horizontal"
        />
      </div>
    </div>

    <div class="row-headers">
      <div v-for="r in level.rows" :key="`row-${r - 1}`" class="header-cell row">
        <CountIndicator
          v-for="color in level.colorCount"
          :key="`row-${r - 1}-${color - 1}`"
          :count="level.rowCounts[color - 1][r - 1]"
          :color="color - 1"
          orientation="vertical"
        />
      </div>
    </div>

    <div class="grid">
      <template v-for="r in level.rows" :key="`r-${r - 1}`">
        <div
          v-for="c in level.cols"
          :key="`cell-${r - 1}-${c - 1}`"
          :class="[
            'cell',
            { blocked: isBlocked(r - 1, c - 1), fixed: fixedAt(r - 1, c - 1) !== null }
          ]"
          :data-color="fixedAt(r - 1, c - 1) ?? undefined"
        >
          <svg v-if="!isBlocked(r - 1, c - 1) && fixedAt(r - 1, c - 1) === null" class="cell-marks" viewBox="0 0 64 64" preserveAspectRatio="xMidYMid meet">
            <!-- corner brackets -->
            <path d="M4 12 L4 4 L12 4" />
            <path d="M52 4 L60 4 L60 12" />
            <path d="M60 52 L60 60 L52 60" />
            <path d="M12 60 L4 60 L4 52" />
            <!-- center cross -->
            <path d="M26 32 L38 32 M32 26 L32 38" />
          </svg>
          <svg v-if="isBlocked(r - 1, c - 1)" class="cell-marks blocked-mark" viewBox="0 0 64 64">
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

/* matte / non-glossy empty cell */
.cell {
  position: relative;
  background: #161a22;
  /* subtle 1px tonal step at the bottom to suggest a panel edge — no shiny gradient */
  border-top: 1px solid rgba(255, 255, 255, 0.025);
  border-bottom: 1px solid rgba(0, 0, 0, 0.5);
  overflow: hidden;
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

/* fixed (immovable) piece — matte fill, sharp border, no glow */
.cell.fixed {
  border-top: none;
  border-bottom: none;
}
.cell.fixed[data-color='0'] {
  background:
    linear-gradient(180deg, #98c52a 0%, #7ba620 50%, #6a8e1c 100%);
  box-shadow:
    inset 0 0 0 1px #c1ec3e,
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
}
.cell.fixed[data-color='1'] {
  background:
    linear-gradient(180deg, #4ca8c4 0%, #347a92 50%, #275f76 100%);
  box-shadow:
    inset 0 0 0 1px #7ed0e6,
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
}
</style>
