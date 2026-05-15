import { ref, watch } from 'vue'

const STORAGE_KEY = 'endfield-bgm-muted'
const VOLUME = 0.35

const muted = ref<boolean>(localStorage.getItem(STORAGE_KEY) === '1')

let el: HTMLAudioElement | null = null

function ensureElement(src: string): HTMLAudioElement {
  if (el) return el
  el = new Audio(src)
  el.loop = true
  el.volume = VOLUME
  el.muted = muted.value
  // Browsers/Electron sometimes refuse autoplay before the first user gesture.
  // We listen once and kick off playback on any pointerdown / keydown.
  const tryPlay = (): void => {
    el?.play().catch(() => {
      /* still blocked, ignore */
    })
  }
  el.play().catch(() => {
    window.addEventListener('pointerdown', tryPlay, { once: true })
    window.addEventListener('keydown', tryPlay, { once: true })
  })
  return el
}

watch(muted, (m) => {
  localStorage.setItem(STORAGE_KEY, m ? '1' : '0')
  if (el) el.muted = m
})

export function useAudio(src: string): {
  muted: typeof muted
  toggle: () => void
  start: () => void
} {
  function start(): void {
    ensureElement(src)
  }
  function toggle(): void {
    muted.value = !muted.value
  }
  return { muted, toggle, start }
}
