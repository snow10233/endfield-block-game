import { ref, watch } from 'vue'

const STORAGE_KEY = 'endfield-bgm-muted'
const BGM_VOLUME = 0.35
const EFFECT_VOLUME = 0.7

const muted = ref<boolean>(localStorage.getItem(STORAGE_KEY) === '1')

let el: HTMLAudioElement | null = null
const effectEls = new Map<string, HTMLAudioElement>()

function ensureElement(src: string): HTMLAudioElement {
  if (el) return el
  el = new Audio(src)
  el.loop = true
  el.volume = BGM_VOLUME
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

function ensureEffect(src: string): HTMLAudioElement {
  const cached = effectEls.get(src)
  if (cached) return cached
  const effect = new Audio(src)
  effect.preload = 'auto'
  effect.volume = EFFECT_VOLUME
  effectEls.set(src, effect)
  return effect
}

watch(muted, (m) => {
  localStorage.setItem(STORAGE_KEY, m ? '1' : '0')
  if (el) el.muted = m
})

export function preloadEffects(sources: string[]): void {
  for (const src of sources) ensureEffect(src)
}

export function playEffect(src: string): void {
  if (muted.value) return
  const base = ensureEffect(src)
  const effect = base.cloneNode(true) as HTMLAudioElement
  effect.volume = EFFECT_VOLUME
  effect.play().catch(() => {
    /* blocked or interrupted, ignore */
  })
}

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
