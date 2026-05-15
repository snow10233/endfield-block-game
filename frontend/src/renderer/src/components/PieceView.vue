<script setup lang="ts">
import type { PieceDef } from '../types/level'

defineProps<{
  piece: PieceDef
  cellSize?: number
}>()
</script>

<template>
  <div
    class="piece"
    :style="{
      '--rows': piece.shape.length,
      '--cols': piece.shape[0].length,
      '--cell-size': `${cellSize ?? 28}px`
    }"
    :data-color="piece.color"
  >
    <div
      v-for="(row, r) in piece.shape"
      :key="r"
      class="piece-row"
    >
      <div
        v-for="(v, c) in row"
        :key="c"
        :class="['piece-cell', { filled: v === 1 }]"
      />
    </div>
  </div>
</template>

<style scoped>
.piece {
  display: flex;
  flex-direction: column;
  gap: 2px;
}
.piece-row {
  display: flex;
  gap: 2px;
}
.piece-cell {
  width: var(--cell-size);
  height: var(--cell-size);
}
.piece-cell.filled {
  background:
    linear-gradient(180deg, var(--top, #98c52a) 0%, var(--mid, #7ba620) 50%, var(--bot, #6a8e1c) 100%);
  box-shadow:
    inset 0 0 0 1px var(--stroke, #c1ec3e),
    inset 0 2px 0 rgba(255, 255, 255, 0.18),
    inset 0 -2px 0 rgba(0, 0, 0, 0.35);
}
.piece[data-color='0'] {
  --top: #98c52a;
  --mid: #7ba620;
  --bot: #6a8e1c;
  --stroke: #c1ec3e;
}
.piece[data-color='1'] {
  --top: #4ca8c4;
  --mid: #347a92;
  --bot: #275f76;
  --stroke: #7ed0e6;
}
</style>
