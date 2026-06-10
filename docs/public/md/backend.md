# 後端邏輯與 Solver 演算法

C++ 後端是本遊戲規則的**唯一真相來源 (Single Source of Truth)**。前端不參與任何關於碰撞、越界或勝利條件的判斷。

## 核心類別 (Core Classes)

* **`Board`**: 負責維護 M x N 的二維陣列。每個 `Cell` 有四種狀態：`Empty` (空格)、`Blocked` (禁區)、`Fixed` (固定色塊)、`Movable` (玩家放置的零件)。
* **`Part`**: 定義單一零件的資料，包含其原始形狀 (2D Vector)、顏色、當前旋轉狀態 (`rotation_`) 以及當前位置。
* **`Game`**: 遊戲的主控制器。負責整合 `Board` 與 `Part`，執行 `canPlace` (碰撞與越界檢查) 與 `isWon` (行列數量驗證) 的核心邏輯。
* **`FileLoader`**: 負責將純文字的關卡設定檔反序列化為 `GameDataPacket` 結構。

## 自動解題器 (Solver)

`solver.cpp` 實作了深度優先的回溯演算法 (Backtracking Depth-First Search) 來自動尋找通關解答。

**效能優化策略 (Pruning & Heuristics):**
1. **依大小排序**：演算法會優先嘗試放置「佔用格數最多」的大型零件，這能有效減少初期的分枝數量。
2. **旋轉去重**：針對對稱圖形（如正方形），會預先剔除重複的旋轉狀態。
3. **提早剪枝 (Early Pruning)**：在嘗試放置零件時，若發現該放置會導致某列/某行的顏色數量**大於**目標需求，便會立刻中斷該分支的探索，大幅提升大棋盤的解題速度。