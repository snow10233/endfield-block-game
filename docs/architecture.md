# Endfield Block Game Architecture

This document summarizes the verified architecture of the repository for
portfolio and interview use. It is based on the checked-in source code under
`backend/`, `frontend/`, and `docs/public/md/`.

## Purpose

Endfield Block Game is a desktop puzzle game where the player places colored
polyomino pieces on a grid. A level is solved when each color satisfies every
row and column count target. The project demonstrates a native C++ game engine
integrated with a modern Electron/Vue desktop interface.

## System Architecture

```mermaid
flowchart TB
  subgraph Frontend["Frontend desktop app"]
    Renderer["Vue renderer<br/>App.vue, GameBoard, PiecePanel, LevelDesigner"]
    Stores["Vue stores<br/>game.ts, drag.ts, audio.ts"]
    Preload["Preload bridge<br/>window.api"]
    Main["Electron main<br/>BrowserWindow, dialogs, BackendBridge"]
  end

  subgraph Backend["C++ backend"]
    MainCpp["main.cpp<br/>JSON request loop"]
    Game["Game<br/>load, place, remove, reset, win check"]
    Board["Board + Cell<br/>grid state"]
    Part["Part<br/>shape, color, rotation, placement"]
    Loader["FileLoader<br/>plain-text level parser"]
    Solver["Solver<br/>backtracking search"]
  end

  Renderer --> Stores
  Stores --> Preload
  Preload --> Main
  Main -->|"stdin JSON line"| MainCpp
  MainCpp --> Game
  Game --> Board
  Game --> Part
  Game --> Loader
  MainCpp --> Solver
  Solver --> Game
  MainCpp -->|"stdout JSON line"| Main
  Main --> Preload
  Preload --> Stores
  Stores --> Renderer
```

Mermaid source: [docs/assets/system-architecture.mmd](assets/system-architecture.mmd)

## Frontend to Backend IPC

```mermaid
sequenceDiagram
  autonumber
  participant UI as Vue renderer
  participant API as Preload window.api
  participant Electron as Electron main
  participant Bridge as BackendBridge
  participant CPP as C++ backend

  UI->>API: backend.call("place", args)
  API->>Electron: ipcRenderer.invoke("backend:call", op, args)
  Electron->>Bridge: call(op, args)
  Bridge->>CPP: {"id":17,"op":"place",...}\n
  CPP->>CPP: validate move and update Game
  CPP-->>Bridge: {"id":17,"ok":true,"won":false}\n
  Bridge-->>Electron: resolve pending request 17
  Electron-->>API: response object
  API-->>UI: typed response
  UI->>API: backend.call("state")
  API-->>UI: refreshed GameState
```

Mermaid source: [docs/assets/ipc-flow.mmd](assets/ipc-flow.mmd)

## Game Flow

```mermaid
flowchart LR
  Start["Start screen"] --> Load["Open level file"]
  Start --> Designer["Level designer"]
  Designer --> Validate["Validate generated level<br/>through C++ solver"]
  Validate --> Export["Export or save level"]
  Validate --> PlayDesigned["Play designed level"]
  Load --> Game["Game screen"]
  PlayDesigned --> Game
  Game --> Drag["Drag piece"]
  Drag --> Rotate["Optional R rotation"]
  Rotate --> Drop["Drop on board"]
  Drag --> Drop
  Drop --> ValidateMove["C++ place validation"]
  ValidateMove -->|invalid| Error["Show error banner"]
  ValidateMove -->|valid| Refresh["Refresh full GameState"]
  Refresh --> WinCheck{"Won?"}
  WinCheck -->|no| Game
  WinCheck -->|yes| Win["Win banner + SFX"]
  Game --> Hint["Hint/auto-solve after 30s"]
  Hint --> Solver["C++ solver"]
  Solver --> Refresh
```

Mermaid source: [docs/assets/game-flow.mmd](assets/game-flow.mmd)

## Verified Runtime Boundary

- `frontend/src/main/index.ts` starts the backend executable as a child process.
- `frontend/src/preload/index.ts` exposes `window.api.backend.call(...)`.
- `frontend/src/renderer/src/api/backend.ts` wraps backend operations into typed
  methods such as `load`, `state`, `place`, `remove`, `solve`, and `autoSolve`.
- `backend/src/main.cpp` reads one request line at a time, switches on `op`, and
  writes one response line with the same `id`.
- `backend/src/game.cpp` owns placement rules and win detection.
- `backend/src/solver.cpp` owns solver search and solution generation.

## Current Protocol Operations

| Operation | Purpose |
| --- | --- |
| `load` | Load a level file from disk. |
| `loadString` | Load a level from generated text. |
| `validateLevelString` | Parse a generated level and verify that the solver can find a solution. |
| `state` | Return the complete current `GameState`. |
| `place` | Attempt to place or rotate a piece at a board coordinate. |
| `remove` | Remove a placed movable piece. |
| `reset` | Restore the current level to its initial state. |
| `solve` | Return one valid solution without applying it. |
| `autoSolve` | Solve and apply the returned placement list to the game. |
| `quit` | Acknowledge and exit the backend loop. |

## Needs Confirmation Before Public Release

- Whether macOS/Linux packaged builds include a matching backend binary. The
  checked-in `electron-builder.yml` explicitly lists `main.exe` for Windows.
- Final screenshot/demo assets. The repository currently contains project
  images and report material, but the portfolio README should use fresh runtime
  screenshots captured from this app.
- Whether the public release should retain the fan-project "Endfield" naming or
  use a more generic portfolio title.

