# 責任邊界與打包部屬 (Deployment)

## 前後端責任邊界 (Separation of Concerns)

* ** 前端禁止行為**：前端絕對不計算「目前放置了幾格」、「是否過關」、「零件能否旋轉放入」。所有的合法性檢查都必須由後端 C++ 提供背書。
* ** 前端允許行為**：滑鼠拖曳的視覺預覽、無效點擊的動畫反饋、音效播放、將 C++ 傳來的 `GameState` 轉化為 HTML DOM。

## 建置與打包說明

本專案使用 `electron-builder` 進行應用程式打包。需要特別注意 C++ 執行檔在開發期與正式包裝後的路徑差異。

**開發模式 (Development)**
前端 `pnpm dev` 預設會尋找相對於專案根目錄的 `../backend/build/main`。因此啟動前端前，必須先執行 CMake 編譯後端。

**正式打包 (Production)**
執行 `pnpm build` 時，`electron-builder.yml` 中的 `extraResources` 設定會自動將編譯好的 C++ 執行檔與預設測試關卡複製到應用程式的 `resources/` 目錄下，確保玩家下載安裝後能直接運作，不需依賴本地端的 C++ 環境。