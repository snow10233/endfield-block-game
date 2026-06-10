# 前端架構 (Electron + Vue 3)

前端的主要責任是「狀態渲染」、「使用者互動」以及「系統檔案存取」。

## 目錄結構

* **`src/main/`**: Electron 的主行程。負責建立視窗、管理與 C++ 的 Child Process 生命週期，以及處理 OS 級別的 Dialog (開啟/儲存檔案)。
* **`src/preload/`**: Context Bridge。提供安全的 API (`window.api`) 供渲染層呼叫，隔離 Node.js 環境。
* **`src/renderer/`**: Vue 3 渲染層。

## Vue 狀態管理與核心元件

本專案利用 Vue 3 的 Composition API (`ref`, `computed`) 進行輕量級的狀態管理，無須引入 Pinia。

* **`store/game.ts`**: 維護從 C++ 取得的最新 `GameState`。
* **`store/drag.ts`**: 獨立管理拖曳狀態。當玩家抓取零件時，此處會計算滑鼠相對於零件中心點的 Offset，並處理 `R` 鍵的旋轉預覽。
* **`GameBoard.vue`**: 渲染棋盤網格。處理 `drop` 事件並計算對應的二維座標。
* **`PiecePanel.vue`**: 顯示未放置的零件列表。
* **`DragLayer.vue`**: 全域的拖曳浮層，利用 CSS `transform: translate` 達到極致的跟隨流暢度。