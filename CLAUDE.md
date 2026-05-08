# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

A school-project mini-game inspired by Arknights: Endfield's 源石電路修復 (Originium Circuit Repair) — a Nonogram-like puzzle where polyomino pieces are placed onto a grid so that each row/column ends up with the required count of colored cells. **Win condition is binary (满足 the row/column counts ⇒ pass); there is no score.** Game-rules reference: `docs/public/pdf/project rule.pdf`.

The user writes in Traditional Chinese and is comfortable with C++ but new to modern frontend tooling (Vue / TypeScript / Electron). Frame frontend explanations from a backend/systems perspective; prefer concrete examples over framework jargon.

## Architecture

The repo is split into two independent projects with distinct toolchains:

```
endfield-block-game/
├── backend/     # C++ — owns ALL game logic (course requirement)
└── frontend/    # Electron + Vue 3 + TypeScript — rendering & input ONLY
```

### Why this split is non-negotiable

**C++ as the source of truth is a hard course requirement.** Do not suggest moving game logic, validation, win-check, level data, or solver code into TypeScript. The frontend exists purely to render state and forward user input.

The course also grades an **auto-solver** (10%) and a **level designer** that exports to the config-file format (10%) — both of these belong in C++ as well.

### Drag-interaction split (important for performance)

The polygon-piece drag UX has two phases that live in different processes:

- **During drag** (per-frame, 60fps): visual follow stays in TS/Konva. Sending every mouse-move event to C++ would saturate IPC and cause lag.
- **On drop** (one-shot): TS sends the final position to C++; C++ is the authority on legality (no overlap, no out-of-bounds, not on a non-placeable cell), snapping to the grid, and the win-check (each row/column matches its required colored-cell count); result comes back and TS reconciles the visual state.

This keeps the "C++ owns logic" contract intact (state authority is in C++) while keeping interaction smooth.

### IPC plan (not yet implemented)

Decided approach: **plain HTTP via `cpp-httplib`** in C++, called from Electron's main process with `fetch`. WebSocket was considered and rejected as overkill — the flow is single-player request/response.

Operational requirements when wiring this up:
- C++ binds **port 0** (OS-assigned) and writes the actual port to stdout. Electron's main process spawns the C++ child, reads stdout to learn the port, then exposes it to the renderer via preload. **Never hardcode a port** — collisions on user machines will brick the app.
- Electron `app.on('before-quit')` must `child.kill()` the C++ process to avoid orphan processes.
- Path to the C++ binary differs between dev and packaged builds: branch on `app.isPackaged` and use `process.resourcesPath` for the packaged case.

For packaging, `electron-builder.yml` will need an `extraResources` entry to bundle the platform-specific C++ binary into the installer.

## Common commands

### Frontend (`cd frontend/`)

Package manager is **pnpm** (not npm). The `.npmrc` sets `node-linker=hoisted` to avoid symlink issues with `electron-builder`; preserve this.

| Command | Purpose |
|---|---|
| `pnpm dev` | Launch Electron with HMR on the renderer |
| `pnpm typecheck` | Run both `typecheck:node` (main/preload) and `typecheck:web` (renderer) |
| `pnpm lint` | ESLint with cache |
| `pnpm format` | Prettier write |
| `pnpm build` | Typecheck + electron-vite build → `out/` |
| `pnpm build:linux` / `:win` / `:mac` | Full platform installer via electron-builder |
| `pnpm build:unpack` | Unpacked build for inspection (no installer) |

The two typecheck targets exist because main/preload run in Node and renderer runs in a browser-like context — they have different lib types and cannot share one tsconfig.

### Backend (`cd backend/`)

CMake + standard out-of-tree build. The executable target is named `endfield`.

```bash
cmake -S . -B build
cmake --build build
./build/endfield
```

`CMAKE_EXPORT_COMPILE_COMMANDS=ON` is set, so `compile_commands.json` lands in `build/` for clangd. The repo-root `.clangd` points there.

## Conventions to preserve

- `.npmrc` settings (`node-linker=hoisted`, `shamefully-hoist=true`) exist specifically for `electron-builder` compatibility with pnpm. Removing them will break packaging.
- `electron.vite.config.ts` defines `@renderer` → `src/renderer/src` alias. Use it instead of long relative paths in renderer code; keep it in sync with `tsconfig.web.json` `paths` if you ever need new aliases.
- `electron-builder.yml` currently uses placeholder `appId: com.electron.app` and `productName: frontend` from the scaffold — update before producing real release builds, but they're fine for development.
- The C++ side intentionally has minimal scaffolding right now (`main.cpp` is a stub). Logic, level data, and the HTTP server are still to be built.

## Out of scope for this project

- Code signing (Mac/Windows) — explicitly skipped; this is a school deliverable, the "unknown publisher" warning is acceptable.
- Cross-platform release builds — build only for the platform being graded on.
- Multi-user / network features — single-player, single-machine.
