<script setup lang="ts">
import type { Level } from '../types/level'
import PieceView from './PieceView.vue'

defineProps<{ level: Level }>()
</script>

<template>
  <div class="piece-panel">
    <div v-for="piece in level.pieces" :key="piece.id" class="slot">
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
  border-radius: 8px;
  background:
    radial-gradient(ellipse at center, rgba(8, 12, 18, 0.5), rgba(4, 6, 10, 0.7));
  box-shadow:
    inset 0 2px 8px rgba(0, 0, 0, 0.5),
    0 0 0 1px rgba(255, 255, 255, 0.03);
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
}
/* same corner-bracket treatment as board cells */
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
