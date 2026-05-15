<script setup lang="ts">
import { ref, computed, watch, onMounted, onUnmounted } from 'vue'
import GameBoard from './components/GameBoard.vue'
import PiecePanel from './components/PiecePanel.vue'
import DragLayer from './components/DragLayer.vue'
import StartScreen from './components/StartScreen.vue'
import LevelDesigner from './components/LevelDesigner.vue'
import { useGame } from './store/game'
import { useDrag } from './store/drag'
import { useViewport, DESIGN_W, DESIGN_H } from './store/viewport'
import { useAudio } from './store/audio'
import bgImage from '@resources/board-background.png'
import bgmSrc from '@resources/bgm.mp3'

type Screen = 'start' | 'game' | 'designer'

const HINT_DELAY_MS = 30_000

const screen = ref<Screen>('start')

const {
  state,
  lastError,
  lastErrorAt,
  loadedAt,
  showHint,
  load,
  loadString,
  reset,
  clear,
  toggleHint
} = useGame()
const { cancel: cancelDrag } = useDrag()
const { scale } = useViewport()
const { muted: bgmMuted, toggle: toggleBgm, start: startBgm } = useAudio(bgmSrc)
onMounted(() => startBgm())

// 30-second gate before the hint becomes available on each new level.
const hintUnlocked = ref(false)
let hintTimer: number | null = null
watch(loadedAt, (val) => {
  if (hintTimer !== null) {
    clearTimeout(hintTimer)
    hintTimer = null
  }
  hintUnlocked.value = false
  if (val > 0) {
    hintTimer = window.setTimeout(() => {
      hintUnlocked.value = true
    }, HINT_DELAY_MS)
  }
})
onUnmounted(() => {
  if (hintTimer !== null) clearTimeout(hintTimer)
})

const errorMsg = computed(() => {
  if (!lastError.value) return null
  const age = Date.now() - lastErrorAt.value
  if (age > 3000) return null
  return lastError.value
})

async function onNewGame(): Promise<void> {
  const path = await window.api.dialog.openLevel()
  if (!path) return
  await load(path)
  if (state.value?.loaded) {
    screen.value = 'game'
  }
}

function onDesigner(): void {
  screen.value = 'designer'
}

function backToMenu(): void {
  cancelDrag()
  clear()
  screen.value = 'start'
}

async function onPlayDesigned(configText: string): Promise<void> {
  await loadString(configText)
  if (state.value?.loaded) {
    screen.value = 'game'
  }
}
</script>

<template>
  <img class="bg" :src="bgImage" alt="" />
  <div class="frost" />

  <div class="viewport">
    <div
      class="stage"
      :style="{
        width: `${DESIGN_W}px`,
        height: `${DESIGN_H}px`,
        transform: `scale(${scale})`
      }"
    >
      <StartScreen
        v-if="screen === 'start'"
        @new-game="onNewGame"
        @designer="onDesigner"
      />

      <div v-else-if="screen === 'game'" class="game-screen">
        <div class="game-content">
          <div v-if="state" class="game">
            <GameBoard />
            <PiecePanel />
          </div>
          <div v-else class="loading">載入中...</div>
        </div>

        <div class="topbar">
          <button class="bar-btn" @click="backToMenu">← 返回</button>
          <button class="bar-btn" @click="reset">重置</button>
          <button
            class="bar-btn"
            :class="{ active: showHint }"
            :disabled="!hintUnlocked"
            :title="hintUnlocked ? '' : '30 秒後解鎖'"
            @click="toggleHint"
          >
            {{ showHint ? '關閉提示' : '提示' }}
          </button>
          <button class="bar-btn" :title="bgmMuted ? '取消靜音' : '靜音'" @click="toggleBgm">
            {{ bgmMuted ? '♪' : '♬' }}
          </button>
        </div>

        <transition name="bounce">
          <div v-if="state?.won" class="win-banner">過關!</div>
        </transition>
      </div>

      <LevelDesigner
        v-else-if="screen === 'designer'"
        @back="backToMenu"
        @play="onPlayDesigned"
      />

      <transition name="fade">
        <div v-if="errorMsg" class="error-banner">{{ errorMsg }}</div>
      </transition>

      <DragLayer />
    </div>
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

.viewport {
  position: fixed;
  inset: 0;
  z-index: 2;
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}

.stage {
  position: relative;
  transform-origin: center center;
  /* fixed design dimensions set inline via :style */
}

/* game screen layout — within the stage */
.game-screen {
  position: absolute;
  inset: 0;
}

.game-content {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 28px;
  padding: 40px;
}

.game {
  display: flex;
  align-items: flex-start;
  gap: 28px;
}

.loading {
  color: #cbd6e2;
  font-size: 16px;
}

.topbar {
  position: absolute;
  top: 20px;
  right: 20px;
  display: flex;
  gap: 8px;
}

.bar-btn {
  padding: 8px 18px;
  background: rgba(14, 18, 26, 0.5);
  color: #cbd6e2;
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 4px;
  font-family: inherit;
  font-size: 14px;
  cursor: pointer;
}
.bar-btn:hover:not(:disabled) {
  background: rgba(30, 40, 60, 0.6);
  border-color: rgba(255, 255, 255, 0.25);
}
.bar-btn:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}
.bar-btn.active {
  background: rgba(184, 232, 53, 0.2);
  border-color: rgba(184, 232, 53, 0.5);
  color: #d8f96a;
}

.error-banner {
  position: absolute;
  top: 20px;
  left: 50%;
  transform: translateX(-50%);
  padding: 10px 20px;
  background: rgba(180, 60, 60, 0.85);
  color: #fff;
  border-radius: 4px;
  font-size: 14px;
  pointer-events: none;
}

.win-banner {
  position: absolute;
  top: 40%;
  left: 50%;
  transform: translateX(-50%);
  z-index: 50;
  padding: 24px 48px;
  background: rgba(184, 232, 53, 0.92);
  color: #1a1f10;
  border-radius: 8px;
  font-size: 36px;
  font-weight: 700;
  letter-spacing: 4px;
  pointer-events: none;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
}

.designer {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
}
.placeholder {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 20px;
  padding: 40px;
  background: rgba(14, 18, 26, 0.5);
  backdrop-filter: blur(8px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 8px;
  color: #cbd6e2;
}
.placeholder h2 {
  margin: 0;
  font-size: 28px;
  letter-spacing: 6px;
}
.placeholder p {
  margin: 0;
  opacity: 0.6;
}

.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s;
}
.fade-enter-from,
.fade-leave-to {
  opacity: 0;
}

/* win-banner uses a snappier scale-in bounce */
.bounce-enter-active {
  transition:
    opacity 0.3s ease,
    transform 0.45s cubic-bezier(0.34, 1.7, 0.64, 1);
}
.bounce-leave-active {
  transition:
    opacity 0.2s,
    transform 0.2s ease-in;
}
.bounce-enter-from {
  opacity: 0;
  transform: translateX(-50%) scale(0.4);
}
.bounce-enter-to {
  opacity: 1;
  transform: translateX(-50%) scale(1);
}
.bounce-leave-from {
  opacity: 1;
  transform: translateX(-50%) scale(1);
}
.bounce-leave-to {
  opacity: 0;
  transform: translateX(-50%) scale(0.85);
}
</style>
