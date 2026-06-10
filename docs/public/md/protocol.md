# 整體資料流與 IPC (Inter-Process Communication)

本專案採用 **NDJSON (Newline Delimited JSON)** 透過標準輸入/輸出 (stdin/stdout) 來達成 Electron (Node.js) 與 C++ 後端的跨行程通訊。

## 架構概念

1. **單向啟動**：Electron 的 Main Process 負責以子行程 (Child Process) 的方式喚起 C++ 執行檔 (`main.exe` 或 `main`)。
2. **非同步對應 (ID Matching)**：因為前端使用者操作與後端運算都是非同步的，每個由前端發出的 Request 都會帶有一個唯一的 `id`。C++ 處理完畢後，會回傳帶有相同 `id` 的 Response，讓 Electron 能準確地 Resolve 對應的 Promise。
3. **效能優化 (拖曳防抖)**：為了保持前端 60FPS 的流暢度，**拖曳過程中的視覺跟隨完全由前端 Vue 處理**。只有在玩家「放開滑鼠 (Drop)」的那一瞬間，前端才會送出 `place` 指令給 C++ 判斷是否合法。

## 資料格式範例

**Frontend Request (前端發送):**
```json
{"id": 1, "op": "place", "pieceId": 0, "row": 2, "col": 3, "rot": 1}
```

**Backend Response (後端回傳):**
```json
{"id": 1, "ok": true, "won": false}
```

## 支援的 IPC 指令 (`op`)
* `load` / `loadString`: 讀取關卡設定檔或字串。
* `state`: 取得當前完整的棋盤與零件狀態。
* `place`: 嘗試在指定座標放置零件。
* `remove`: 移除已放置的零件。
* `solve` / `autoSolve`: 呼叫後端演算法尋找解答。
* `validateLevelString`: 用於關卡設計器，驗證自訂關卡是否有解