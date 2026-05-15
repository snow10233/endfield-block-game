<script setup lang="ts">
const emit = defineEmits<{
  newGame: []
  designer: []
}>()
</script>

<template>
  <div class="start-screen">
    <div class="title-block">
      <div class="subtitle">明日方舟 終末地</div>
      <h1 class="title">源石電路修復</h1>
      <div class="divider" />
    </div>

    <div class="menu">
      <button class="menu-btn" @click="emit('newGame')">
        <span class="btn-label">開啟新遊戲</span>
        <span class="btn-sub">load level file</span>
      </button>
      <button class="menu-btn" @click="emit('designer')">
        <span class="btn-label">關卡設計</span>
        <span class="btn-sub">level designer</span>
      </button>
    </div>
  </div>
</template>

<style scoped>
.start-screen {
  position: absolute;
  inset: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 80px;
  padding: 40px;
}

.title-block {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
}

.subtitle {
  font-size: 14px;
  letter-spacing: 8px;
  color: rgba(184, 232, 53, 0.7);
  text-transform: uppercase;
  font-family: 'JetBrains Mono', 'Consolas', monospace;
}

.title {
  margin: 0;
  font-size: 56px;
  font-weight: 700;
  letter-spacing: 12px;
  color: #e6efea;
  text-shadow: 0 2px 12px rgba(0, 0, 0, 0.6);
}

.divider {
  width: 240px;
  height: 1px;
  background: linear-gradient(90deg, transparent, rgba(184, 232, 53, 0.6), transparent);
}

.menu {
  display: flex;
  flex-direction: column;
  gap: 16px;
  min-width: 320px;
}

.menu-btn {
  position: relative;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  gap: 4px;
  padding: 18px 28px;
  background: rgba(14, 18, 26, 0.45);
  backdrop-filter: blur(8px);
  border: 1px solid rgba(184, 232, 53, 0.25);
  border-radius: 4px;
  color: #cbd6e2;
  font-family: inherit;
  text-align: left;
  cursor: pointer;
  transition:
    background 0.15s ease,
    border-color 0.15s ease,
    transform 0.1s ease;
}

.menu-btn::before,
.menu-btn::after {
  content: '';
  position: absolute;
  width: 10px;
  height: 10px;
  border: 1px solid rgba(184, 232, 53, 0.5);
  pointer-events: none;
}
.menu-btn::before {
  top: -1px;
  left: -1px;
  border-right: none;
  border-bottom: none;
}
.menu-btn::after {
  bottom: -1px;
  right: -1px;
  border-left: none;
  border-top: none;
}

.menu-btn:hover {
  background: rgba(30, 40, 30, 0.55);
  border-color: rgba(184, 232, 53, 0.6);
}
.menu-btn:active {
  transform: translateY(1px);
}

.btn-label {
  font-size: 20px;
  font-weight: 600;
  letter-spacing: 4px;
  color: #b8e835;
}
.btn-sub {
  font-size: 11px;
  letter-spacing: 2px;
  color: rgba(203, 214, 226, 0.5);
  font-family: 'JetBrains Mono', 'Consolas', monospace;
  text-transform: uppercase;
}
</style>
