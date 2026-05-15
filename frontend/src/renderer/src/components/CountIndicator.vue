<script setup lang="ts">
defineProps<{
  count: number
  color: number
  orientation: 'horizontal' | 'vertical'
}>()
</script>

<template>
  <div :class="['count-indicator', orientation]" :data-color="color">
    <span v-for="i in count" :key="i" class="bar" />
  </div>
</template>

<style scoped>
.count-indicator {
  display: flex;
  gap: 3px;
  align-items: center;
  justify-content: center;
}
.count-indicator.horizontal {
  flex-direction: column;
}
.count-indicator.vertical {
  flex-direction: row;
}
.bar {
  display: block;
  background: transparent;
  border: 1px solid var(--piece-color, #b8e835);
  border-radius: 1px;
}
.horizontal .bar {
  width: 18px;
  height: 4px;
}
.vertical .bar {
  width: 4px;
  height: 18px;
}
.count-indicator[data-color='0'] {
  --piece-color: #b8e835;
}
.count-indicator[data-color='1'] {
  --piece-color: #4ec0e0;
}
</style>
