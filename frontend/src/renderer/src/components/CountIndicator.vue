<script setup lang="ts">
import { computed } from 'vue'

const props = defineProps<{
  count: number
  filled?: number
  color: number
  orientation: 'horizontal' | 'vertical'
}>()

const status = computed<'pending' | 'ok' | 'over'>(() => {
  const f = props.filled ?? 0
  if (f === props.count) return 'ok'
  if (f > props.count) return 'over'
  return 'pending'
})

// Number of filled bars to render solid (others hollow)
const filledBars = computed(() => Math.min(props.filled ?? 0, props.count))
</script>

<template>
  <div
    :class="['count-indicator', orientation, status]"
    :data-color="color"
    :title="`${filled ?? 0} / ${count}`"
  >
    <span
      v-for="i in count"
      :key="i"
      :class="['bar', { 'bar-filled': i <= filledBars }]"
    />
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
  transition: background 0.15s ease;
}
.bar-filled {
  background: var(--piece-color, #b8e835);
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
/* satisfied: brighter outline (already shows as fully filled) — handled by bar-filled */
.count-indicator.ok .bar {
  border-color: var(--piece-color, #b8e835);
  filter: brightness(1.05);
}
.count-indicator.over .bar,
.count-indicator.over .bar-filled {
  border-color: #e85050;
  background: #e85050;
}
</style>
