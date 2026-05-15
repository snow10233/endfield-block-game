<script setup lang="ts">
import GameBoard from './components/GameBoard.vue'
import PiecePanel from './components/PiecePanel.vue'
import { example1 } from './data/example1'
import bgImage from '@resources/board-background.png'
</script>

<template>
  <img class="bg" :src="bgImage" alt="" />
  <div class="frost" />
  <div class="stage">
    <GameBoard :level="example1" />
    <PiecePanel :level="example1" />
  </div>
</template>

<style scoped>
.bg {
  position: fixed;
  inset: 0;
  width: 100vw;
  height: 100vh;
  object-fit: cover;
  z-index: 0;
  pointer-events: none;
}

.frost {
  position: fixed;
  inset: 0;
  z-index: 1;
  pointer-events: none;
  background: rgba(14, 18, 26, 0.1);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
}

.stage {
  position: relative;
  z-index: 2;
  display: flex;
  align-items: flex-start;
  gap: 28px;
  padding: 40px;
}
</style>
