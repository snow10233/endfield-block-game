# Endfield Block Game 代碼說明

此 repo 是一個以《明日方舟：終末地》「源石電路修復」為靈感的拼圖小遊戲。玩法接近 Nonogram：玩家把不同形狀的方塊零件放到棋盤上，讓每一列、每一行的指定顏色格數都符合提示。只要列與行的數量全部符合就過關，沒有分數系統。

專案分成兩個獨立工程：

```text
endfield-block-game/
├── backend/      C++20 + CMake，負責所有遊戲規則、關卡解析、勝利判定與解題器
├── frontend/     Electron + Vue 3 + TypeScript，負責畫面、拖曳操作、音效與檔案對話框
├── docs/         規則與素材文件
└── intro.md      本文件
```

核心設計原則是：**後端 C++ 是遊戲邏輯的唯一真相來源**。前端可以做拖曳時的即時視覺預覽，但真正能不能放置、是否過關、是否有解，都要交給 C++ 判斷。

## 整體資料流

執行 Electron 前端時，Electron main process 會啟動後端 C++ 執行檔 `backend/build/main`。兩者用 stdin/stdout 傳 NDJSON，也就是一行一個 JSON 物件。

```text
Vue renderer
  ↓ window.api.backend.call(...)
preload bridge
  ↓ ipcRenderer.invoke('backend:call')
Electron main process
  ↓ child.stdin.write(JSON line)
C++ backend process
  ↓ stdout 回傳 JSON line
Electron main process
  ↓ 依 id 找回 pending Promise
Vue renderer 更新畫面
```

例如放置零件時：

```json
{"id":1,"op":"place","pieceId":0,"row":0,"col":0,"rot":0}
```

C++ 回傳：

```json
{"id":1,"ok":true,"won":false}
```

前端在拖曳過程中只處理視覺跟隨與預覽；滑鼠放開時才把最終格子座標送到後端。這樣可以避免每一個 mousemove 都跨 process 呼叫，拖曳會比較順。

## 後端 C++ 說明

後端位於 `backend/`，使用 CMake 建置，主要 target 是：

- `core`：遊戲核心 library，包含棋盤、零件、關卡載入、遊戲規則與 solver。
- `main`：stdin/stdout IPC 入口，Electron 實際啟動的是這個執行檔。

主要檔案如下：

| 檔案 | 責任 |
|---|---|
| `include/cell.h` | 定義棋盤格狀態：空格、不可放置格、固定色塊、玩家放置的可移動零件 |
| `include/board.h`, `src/board.cpp` | 管理 M x N 棋盤與格子存取 |
| `include/part.h`, `src/part.cpp` | 管理可移動零件的形狀、顏色、旋轉、目前放置位置 |
| `include/file-loader.h`, `src/file-loader.cpp` | 解析關卡文字檔，轉成 `GameDataPacket` |
| `include/game.h`, `src/game.cpp` | 遊戲規則核心：載入、重置、放置、移除、勝利判定、目前列行計數 |
| `include/solver.h`, `src/solver.cpp` | 回溯法解題器，找出一組可過關的零件擺法 |
| `include/json.h` | 輕量 JSON 輸出與簡單 top-level 欄位解析 |
| `src/main.cpp` | IPC 指令入口，讀 request、呼叫 `Game` 或 `Solver`、輸出 response |

### `Cell` 與 `Board`

`CellState` 有四種：

- `Empty`：可以放零件。
- `Blocked`：不可放置，對應關卡檔中的非顏色固定格。
- `Fixed`：關卡一開始就固定的有色格，不能被玩家移動。
- `Movable`：玩家目前放上去的零件格。

`Board` 只負責存放 `vector<vector<Cell>>`，提供 `resize()`、`inBounds()`、`at()`。它本身不判斷遊戲規則，這些都集中在 `Game`。

### `Part`

`Part` 表示一個可移動零件，包含：

- `id_`：零件編號。
- `color_`：零件顏色。
- `baseShape_`：未旋轉形狀，二維陣列中 `1` 表示該格有方塊。
- `rotation_`：目前旋轉，範圍是 `0..3`，每次代表順時針 90 度。
- `placed_`, `placedRow_`, `placedCol_`：是否已放到棋盤，以及旋轉後 bounding box 左上角位置。

`getCurrentShape()` 會依照 `rotation_` 回傳旋轉後的形狀。`getCurrentMid()` 會把原始形狀中第一個填滿格當作拖曳/旋轉錨點，再映射到旋轉後座標，讓前後端對「滑鼠指向零件哪一格」有一致理解。

### `FileLoader` 與關卡資料

`FileLoader` 可以從檔案或字串讀取關卡設定。解析後存進 `GameDataPacket`：

```cpp
struct GameDataPacket {
  int colors, row, col;
  std::vector<OneColorRule> rules;
  std::vector<Position> nonColorFixeds;
  std::vector<Part> partDatas;
};
```

每個顏色都有一組 `OneColorRule`：

- `rowNeed`：該顏色每列需要幾格。
- `colNeed`：該顏色每行需要幾格。
- `fixeds`：該顏色的固定格座標。

### `Game`

`Game` 是後端最重要的類別，負責把關卡資料變成可操作的遊戲狀態。

主要流程：

1. `load()` 或 `loadFromString()` 讀入關卡。
2. `applyLoaded()` 建立棋盤、複製零件、套用固定格與 blocked 格。
3. `placePart(pieceId, row, col, rotation)` 嘗試放置零件。
4. `canPlace()` 檢查是否越界、是否碰到 blocked/fixed/其他 movable 格。
5. 放置成功後 `writePieceCells()` 寫入棋盤。
6. `isWon()` 檢查每個顏色的每列、每行數量是否完全符合目標。

`Game` 的勝利條件是嚴格相等：某顏色在某列/某行的目前格數必須等於關卡指定值。只要有一列或一行不相等，就還沒過關。

### `Solver`

`Solver` 是自動解題器，使用回溯搜尋：

1. 從關卡初始狀態建立暫存棋盤，固定格與 blocked 格會被保留。
2. 產生每個零件的不同旋轉形狀；對稱旋轉會去重，減少搜尋量。
3. 依零件填滿格數由大到小排序，先放限制較大的零件。
4. 遞迴嘗試每個零件的所有旋轉與所有位置。
5. 每次嘗試時檢查是否越界、重疊，以及是否超過 row/col 目標。
6. 全部零件放完後，檢查所有 row/col count 是否剛好符合。

`solve()` 只找一組解，成功後會依 `pieceId` 排序輸出：

```cpp
struct SolverPlacement {
  int pieceId;
  int row;
  int col;
  int rotation;
};
```

### C++ IPC 指令

`backend/src/main.cpp` 讀取每一行 JSON request，依照 `op` 呼叫對應功能：

| op | 功能 |
|---|---|
| `load` | 從檔案路徑載入關卡 |
| `loadString` | 從前端傳來的文字載入關卡 |
| `validateLevelString` | 驗證關卡文字格式，並用 solver 確認是否有解 |
| `state` | 回傳完整遊戲狀態給前端 |
| `place` | 放置或移動某個零件 |
| `remove` | 把已放置零件移回零件欄 |
| `reset` | 重置目前關卡 |
| `solve` | 回傳 solver 找到的解，但不改變棋盤 |
| `autoSolve` | 找解後直接套用到目前棋盤 |
| `quit` | 回傳成功並結束後端 process |

回傳資料都會包含 request 的 `id`，Electron main process 用這個 `id` 找回對應 Promise。

## 前端 Electron/Vue 說明

前端位於 `frontend/`，由三層組成：

```text
Electron main process     src/main/index.ts
preload bridge            src/preload/index.ts
Vue renderer              src/renderer/src/*
```

### Electron main process

`frontend/src/main/index.ts` 做幾件事：

- 建立主視窗 `BrowserWindow`。
- 依開發模式或打包模式決定 C++ binary 路徑。
- 用 `spawn()` 啟動 `backend/build/main`。
- 用 `readline` 逐行讀取後端 stdout。
- 建立 `BackendBridge`，用 request `id` 管理 pending Promise。
- 提供 IPC handler：
  - `backend:call`：renderer 呼叫後端 C++。
  - `dialog:openLevel`：開啟關卡檔案。
  - `dialog:saveLevel`：存出關卡設計器產生的文字。
- 在 `before-quit` 時 kill 後端 process，避免殘留。

### Preload bridge

`frontend/src/preload/index.ts` 透過 `contextBridge.exposeInMainWorld()` 暴露安全 API：

```ts
window.api.backend.call(op, args)
window.api.dialog.openLevel()
window.api.dialog.saveLevel(text)
```

Vue renderer 不會直接碰 Node.js、`child_process` 或 Electron main object，而是只透過這層 bridge 呼叫。

### Renderer API wrapper

`frontend/src/renderer/src/api/backend.ts` 把通用的 `window.api.backend.call()` 包成型別化方法：

- `load(path)`
- `loadString(text)`
- `validateLevelString(text)`
- `state()`
- `place(pieceId, row, col, rot)`
- `remove(pieceId)`
- `reset()`
- `solve()`
- `autoSolve()`

型別定義在 `frontend/src/renderer/src/types/level.ts`，例如 `GameState`、`MovablePiece`、`PlaceResponse`、`SolveResponse`。

### Vue store

此專案沒有使用 Pinia，而是用 Vue module singleton + `ref/computed` 實作小型 store。

| 檔案 | 功能 |
|---|---|
| `store/game.ts` | 保存目前 `GameState`、錯誤訊息、solver 解、提示狀態、載入時間、是否正在套用解 |
| `store/drag.ts` | 保存拖曳中的零件、滑鼠位置、旋轉狀態，也提供形狀旋轉與座標旋轉工具 |
| `store/viewport.ts` | 固定 1100 x 800 設計座標，依視窗大小縮放，並把 client 座標轉回 stage 座標 |
| `store/audio.ts` | 背景音樂、音效快取、靜音狀態與 localStorage |

`store/game.ts` 的重要方法：

- `load()` / `loadString()`：載入關卡後呼叫 `refresh()`。
- `refresh()`：向後端要最新 `state`。
- `place()`：呼叫後端 `place`，成功後刷新畫面並觸發放置動畫。
- `fetchSolution()`：呼叫後端 `solve` 取得提示用解答。
- `autoPlaceSolution()`：呼叫後端 `autoSolve`，讓 C++ 找解並套用。

### 主要 Vue 元件

| 元件 | 功能 |
|---|---|
| `App.vue` | 最上層畫面狀態，切換 start/game/designer，處理 topbar、提示解鎖、音效與勝利 banner |
| `StartScreen.vue` | 起始選單，提供開啟新遊戲與關卡設計 |
| `GameBoard.vue` | 棋盤、列行提示、格子狀態、放置預覽、solver hint overlay、從棋盤拖起已放置零件 |
| `PiecePanel.vue` | 右側零件欄，顯示所有零件；未放置零件可拖曳，已放置零件顯示為灰色 placeholder |
| `DragLayer.vue` | 全域拖曳浮層，跟隨滑鼠；按 `R` 旋轉，按 `Escape` 可取消或移除已放置零件 |
| `PieceView.vue` | 單純負責畫出一個零件形狀 |
| `CountIndicator.vue` | 顯示每列/每行目標數量與目前完成數量 |
| `LevelDesigner.vue` | 關卡設計器，可編輯棋盤、固定格、blocked 格、目標數量、零件形狀，並輸出設定檔 |

## 玩家操作流程

### 載入關卡

1. 玩家在 `StartScreen` 點「開啟新遊戲」。
2. `App.vue` 呼叫 `window.api.dialog.openLevel()`。
3. Electron main process 開啟檔案選擇對話框。
4. 選到檔案後，`store/game.ts` 呼叫 `backend.load(path)`。
5. C++ `Game::load()` 解析檔案，建立棋盤與零件。
6. 前端呼叫 `state()`，用回傳的 `GameState` 渲染畫面。

### 拖曳放置零件

1. 玩家從 `PiecePanel` 按住未放置零件，或從 `GameBoard` 按住已放置零件。
2. `store/drag.ts` 記錄拖曳狀態。
3. `DragLayer` 讓零件跟隨滑鼠；按 `R` 只改前端視覺旋轉狀態。
4. `GameBoard` 根據滑鼠位置計算 drop cell，並顯示綠色/紅色 preview。
5. 滑鼠放開時，前端計算零件旋轉後 bounding box 左上角。
6. 前端呼叫 `backend.place(pieceId, row, col, rot)`。
7. C++ 檢查合法性，成功後更新棋盤並判斷是否過關。
8. 前端刷新 `state()` 並重新渲染。

### 提示與自動解題

`App.vue` 在關卡載入後設定 30 秒計時器。30 秒後才允許使用提示與 AI 自動放置。

- 點「提示」：前端呼叫 `solve()`，只取得 solver 結果，再由 `GameBoard` 在空格上畫 hint overlay。
- 點「AI幫你放」：前端呼叫 `autoSolve()`，後端找解並直接套用到遊戲狀態。

### 關卡設計器

`LevelDesigner.vue` 可以：

- 調整棋盤 rows/cols。
- 設定顏色數，目前 UI 支援 1 或 2 色。
- 編輯每列/每行的目標數字。
- 放置 blocked 格與固定有色格。
- 新增、刪除與編輯零件形狀。
- 產生與後端 `FileLoader` 相容的文字格式。
- 呼叫 `validateLevelString()`，用 C++ parser + solver 確認格式正確且關卡有解。
- 通過驗證後才能輸出文字或存檔。

## 關卡文字格式

關卡檔是純文字。`FileLoader::parse()` 依序讀取：

```text
colors rows cols

每個 color:
  rowNeed[0..rows-1]
  colNeed[0..cols-1]
  fixedCount
  fixed row col
  fixed row col
  ...

nonColorFixedCount
blocked row col
blocked row col
...

每個 movable part，直到 EOF:
  color partRows partCols
  shape row 0
  shape row 1
  ...
```

範例：

```text
1 3 3
3 2 1
3 2 1
0
0
0 2 2
1 1
1 0
0 1 3
1 1 1
```

意思是：

- 1 種顏色，棋盤 3 x 3。
- 這個顏色每列需要 `3, 2, 1` 格，每行需要 `3, 2, 1` 格。
- 沒有固定有色格。
- 沒有 blocked 格。
- 有兩個零件：
  - color 0，2 x 2，形狀是 `[[1,1],[1,0]]`
  - color 0，1 x 3，形狀是 `[[1,1,1]]`

## 建置與執行

後端：

```bash
cd backend
cmake -S . -B build
cmake --build build
./build/main
```

前端：

```bash
cd frontend
pnpm install
pnpm dev
```

型別檢查與打包：

```bash
cd frontend
pnpm typecheck
pnpm build
pnpm build:linux
pnpm build:win
pnpm build:mac
```

注意：前端開發模式會找 `../backend/build/main`，所以通常要先把後端 build 起來，再跑 `pnpm dev`。

## 打包設定

`frontend/electron-builder.yml` 已設定：

- `appId: com.endfield.blockgame`
- `productName: EndfieldBlockGame`
- `extraResources` 會把 `backend/tests` 放進 packaged app。
- Windows build 會額外把 `../backend/build/main.exe` 放到 `resources/backend/main.exe`。

開發模式與打包模式的後端路徑不同：

- 開發模式：`backend/build/main`
- 打包模式：`process.resourcesPath/backend/main` 或 Windows 的 `main.exe`

如果要在非 Windows 平台正式打包，也要確認對應平台的 C++ binary 有被放進 `extraResources`。

## 程式責任邊界

這個專案最重要的維護原則：

- C++ 後端負責規則、驗證、勝利判定、solver、關卡格式。
- Vue/TypeScript 前端負責畫面、輸入、拖曳預覽、音效、視窗與檔案對話框。
- 前端可以做「預覽是否合法」的視覺提示，但最終結果仍以 C++ `place` 回傳為準。
- 如果之後要新增規則，例如更多顏色、特殊格、不同勝利條件，應先改 C++ 的資料結構與 `Game`/`Solver`，再讓前端顯示新狀態。
