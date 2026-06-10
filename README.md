# Endfield Block Game

使用 C++ 後端引擎與 Electron (Vue 3) 前端的拼圖小遊戲專案。

## Quick Start

1. 建置後端：進入 `backend` 後執行 `cmake -S . -B build` 與 `cmake --build build`
2. 安裝套件：進入 `frontend` 後執行 `pnpm install`
3. 啟動前端：在 `frontend` 目錄下執行 `pnpm dev`

## Documentation

- [整體資料流與 IPC](docs/public/md/protocol.md)
- [後端邏輯與 Solver](docs/public/md/backend.md)
- [前端架構](docs/public/md/frontend.md)
- [關卡文字格式](docs/public/md/level-format.md)
- [關卡設計器](docs/public/md/designer.md)
- [責任邊界與打包](docs/public/md/development.md)
- [系統邊界與防呆測試報告](backend/tests/more_tests/edge-cases.md)
