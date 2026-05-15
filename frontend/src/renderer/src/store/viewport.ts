import { ref, onMounted, onUnmounted, type Ref } from 'vue'

export const DESIGN_W = 1100
export const DESIGN_H = 800

const scale = ref(1)

function update(): void {
  scale.value = Math.min(window.innerWidth / DESIGN_W, window.innerHeight / DESIGN_H)
}

let mounts = 0
function ensureListener(): void {
  if (mounts++ === 0) {
    update()
    window.addEventListener('resize', update)
  }
}
function releaseListener(): void {
  if (--mounts === 0) {
    window.removeEventListener('resize', update)
  }
}

// Convert a viewport-space (clientX/Y) coordinate to design-space (the
// coordinate system inside the scaled stage). Since the stage is centered,
// the origin is at ((winW - DESIGN_W*scale)/2, (winH - DESIGN_H*scale)/2).
function toStageLocal(clientX: number, clientY: number): { x: number; y: number } {
  const stageW = DESIGN_W * scale.value
  const stageH = DESIGN_H * scale.value
  const originX = (window.innerWidth - stageW) / 2
  const originY = (window.innerHeight - stageH) / 2
  return {
    x: (clientX - originX) / scale.value,
    y: (clientY - originY) / scale.value
  }
}

export function useViewport(): {
  scale: Ref<number>
  toStageLocal: typeof toStageLocal
} {
  onMounted(ensureListener)
  onUnmounted(releaseListener)
  return { scale, toStageLocal }
}

// For components that need scale outside of a setup() context (rare).
export function getScale(): number {
  return scale.value
}
