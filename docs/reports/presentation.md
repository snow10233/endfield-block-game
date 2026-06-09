---
marp: true
theme: default
paginate: true
size: 16:9
footer: Endfield Block Game
style: |
  :root {
    --bg: #181716;
    --panel: #22201f;
    --ink: #f4efe9;
    --muted: #b9ada3;
    --line: #3b3632;
    --orange: #d97757;
    --orange-2: #f0a37f;
    --green: #a6d86f;
    --red: #e46f64;
  }
  section {
    font-family: "Noto Sans TC", "Microsoft JhengHei", system-ui, sans-serif;
    background:
      radial-gradient(circle at 86% 12%, rgba(217, 119, 87, 0.2), transparent 30%),
      linear-gradient(135deg, #181716 0%, #211f1d 100%);
    color: var(--ink);
    padding: 50px 64px;
    letter-spacing: 0;
  }
  section::after {
    color: var(--muted);
    font-size: 17px;
  }
  h1,
  h2,
  h3,
  p {
    margin: 0;
  }
  h1 {
    color: var(--orange-2);
    font-size: 70px;
    line-height: 1.05;
    letter-spacing: 0;
  }
  h2 {
    color: var(--orange-2);
    font-size: 44px;
    line-height: 1.15;
    margin-bottom: 28px;
    letter-spacing: 0;
  }
  h3 {
    color: var(--ink);
    font-size: 27px;
    line-height: 1.25;
    margin-bottom: 10px;
  }
  p,
  li {
    font-size: 25px;
    line-height: 1.45;
  }
  ul,
  ol {
    margin: 0;
    padding-left: 1.15em;
  }
  li + li {
    margin-top: 9px;
  }
  strong {
    color: var(--orange-2);
  }
  code {
    padding: 0.08em 0.3em;
    border-radius: 5px;
    background: #302c29;
    color: var(--orange-2);
    font-size: 0.86em;
  }
  pre {
    margin: 0;
    padding: 18px 20px;
    border: 1px solid var(--line);
    border-radius: 10px;
    background: #121110;
  }
  pre code {
    padding: 0;
    background: transparent;
    color: #f2e5d8;
    font-size: 21px;
  }
  .kicker {
    color: var(--orange);
    font-size: 21px;
    font-weight: 800;
    letter-spacing: 0.08em;
    text-transform: uppercase;
  }
  .sub {
    color: var(--muted);
    font-size: 28px;
    line-height: 1.45;
    margin-top: 20px;
  }
  .small {
    color: var(--muted);
    font-size: 21px;
  }
  .title-grid,
  .split {
    display: grid;
    grid-template-columns: 1.05fr 0.95fr;
    gap: 42px;
    align-items: center;
    height: 100%;
  }
  .two {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 28px;
  }
  .three {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 20px;
  }
  .quad {
    display: grid;
    grid-template-columns: 1fr 1fr;
    grid-template-rows: 1fr 1fr;
    gap: 18px;
  }
  .panel,
  .card {
    border: 1px solid var(--line);
    border-radius: 14px;
    background: linear-gradient(180deg, var(--panel) 0%, #1c1a19 100%);
    box-shadow: 0 14px 34px rgba(0, 0, 0, 0.22);
  }
  .panel {
    padding: 26px;
  }
  .card {
    min-height: 178px;
    padding: 22px;
  }
  .card p,
  .panel p {
    color: var(--muted);
    font-size: 22px;
  }
  .pill {
    display: inline-block;
    padding: 6px 12px;
    border: 1px solid rgba(217, 119, 87, 0.65);
    border-radius: 999px;
    color: var(--orange-2);
    background: rgba(217, 119, 87, 0.12);
    font-size: 18px;
    font-weight: 700;
    margin-bottom: 12px;
  }
  .flow {
    display: flex;
    align-items: center;
    gap: 12px;
    margin-top: 18px;
  }
  .node {
    flex: 1;
    min-height: 96px;
    padding: 16px 14px;
    border: 1px solid var(--line);
    border-radius: 12px;
    background: var(--panel);
    text-align: center;
  }
  .node b {
    display: block;
    color: var(--orange-2);
    font-size: 23px;
  }
  .node span {
    color: var(--muted);
    font-size: 17px;
  }
  .arrow {
    color: var(--orange);
    font-size: 31px;
    font-weight: 900;
  }
  .big {
    color: var(--orange-2);
    font-size: 56px;
    font-weight: 900;
    line-height: 1;
  }
  .mock {
    position: relative;
    min-height: 410px;
    overflow: hidden;
  }
  .board {
    position: absolute;
    left: 38px;
    top: 34px;
    display: grid;
    grid-template-columns: repeat(5, 54px);
    grid-template-rows: repeat(5, 54px);
    gap: 8px;
  }
  .cell {
    border: 1px solid #4b4540;
    border-radius: 8px;
    background: #151413;
  }
  .cell.o {
    background: linear-gradient(135deg, var(--orange), #9d4f38);
  }
  .cell.g {
    background: linear-gradient(135deg, var(--green), #547b31);
  }
  .cell.b {
    background: #312d2a;
  }
  .piece {
    position: absolute;
    right: 46px;
    bottom: 48px;
    display: grid;
    grid-template-columns: repeat(3, 46px);
    grid-template-rows: repeat(2, 46px);
    gap: 7px;
    transform: rotate(-10deg);
  }
  .pc {
    border-radius: 8px;
    background: var(--orange-2);
    box-shadow: 0 10px 22px rgba(217, 119, 87, 0.24);
  }
  .pc.blank {
    background: transparent;
    box-shadow: none;
  }
  .hintline {
    position: absolute;
    right: 42px;
    top: 58px;
    width: 220px;
    color: var(--muted);
    font-size: 23px;
  }
  .check {
    color: var(--green);
    font-weight: 900;
  }
  .bad {
    color: var(--red);
    font-weight: 900;
  }
---

<!-- _class: lead -->

<div class="title-grid">
  <div>
    <div class="kicker">Puzzle Game / Desktop App</div>
    <h1>Endfield<br />Block Game</h1>
    <p class="sub">以「源石電路修復」為靈感的 Nonogram-like 拼圖小遊戲</p>
  </div>
  <div class="panel mock">
    <div class="board">
      <div class="cell o"></div><div class="cell o"></div><div class="cell"></div><div class="cell g"></div><div class="cell"></div>
      <div class="cell"></div><div class="cell o"></div><div class="cell b"></div><div class="cell g"></div><div class="cell g"></div>
      <div class="cell"></div><div class="cell"></div><div class="cell o"></div><div class="cell"></div><div class="cell"></div>
      <div class="cell g"></div><div class="cell b"></div><div class="cell"></div><div class="cell o"></div><div class="cell"></div>
      <div class="cell"></div><div class="cell"></div><div class="cell g"></div><div class="cell"></div><div class="cell o"></div>
    </div>
    <div class="hintline">row / col count<br />+ polyomino pieces</div>
    <div class="piece">
      <div class="pc"></div><div class="pc"></div><div class="pc blank"></div>
      <div class="pc blank"></div><div class="pc"></div><div class="pc"></div>
    </div>
  </div>
</div>

---

## 遊戲目標

<div class="split">
  <div>
    <p class="kicker">Core Rule</p>
    <h1 style="font-size: 54px">把零件放對，讓提示數字成立</h1>
  </div>
  <div class="panel">
    <ul>
      <li>拖曳多邊形零件到棋盤</li>
      <li>每列、每行都有指定顏色格數</li>
      <li>所有 row / col count <strong>剛好相等</strong>就過關</li>
      <li>沒有分數，只有 pass / not yet</li>
    </ul>
  </div>
</div>

---

## 技術選型與責任

<div class="three">
  <div class="card">
    <span class="pill">Backend</span>
    <h3>C++20 + CMake</h3>
    <p>關卡解析、規則判斷、勝利條件、solver。</p>
  </div>
  <div class="card">
    <span class="pill">Desktop</span>
    <h3>Electron</h3>
    <p>桌面視窗、檔案對話框、啟動 C++ process。</p>
  </div>
  <div class="card">
    <span class="pill">UI</span>
    <h3>Vue 3 + TypeScript</h3>
    <p>畫面、拖曳、提示 overlay、音效、關卡設計器。</p>
  </div>
</div>

<p class="sub">重點：不使用遊戲引擎，因為本專案更像規則明確的互動工具。</p>

---

## 架構：C++ 是唯一真相來源

<div class="flow">
  <div class="node"><b>Vue Renderer</b><span>畫面與輸入</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>Preload</b><span>安全 API bridge</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>Electron Main</b><span>spawn + IPC</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>C++ Backend</b><span>規則唯一真相</span></div>
</div>

<div class="two" style="margin-top: 30px">
  <div class="panel">
    <h3>前端負責體驗</h3>
    <p>rendering、拖曳預覽、音效、檔案對話框、designer UI。</p>
  </div>
  <div class="panel">
    <h3>後端負責規則</h3>
    <p>load、place、remove、win check、solve、level validation。</p>
  </div>
</div>

---

## IPC 與拖曳設計

<div class="two">
  <div class="panel">
    <h3>NDJSON over stdin/stdout</h3>
    <pre><code>{"id":1,"op":"place",
 "pieceId":0,
 "row":2,"col":3,"rot":1}</code></pre>
    <p style="margin-top: 16px">每個 request 用 <code>id</code> 對回 response。</p>
  </div>
  <div class="panel">
    <h3>拖曳不每幀問 C++</h3>
    <ul>
      <li>拖曳中：前端即時預覽</li>
      <li>放開時：C++ 判斷合法性</li>
      <li>成功後：重新抓後端 state</li>
    </ul>
  </div>
</div>

---

## 核心功能

<div class="quad">
  <div class="card">
    <h3>關卡格式</h3>
    <p>純文字描述棋盤、row/col 目標、固定格、不可放置格與零件。</p>
  </div>
  <div class="card">
    <h3>放置判斷</h3>
    <p>檢查越界、blocked、fixed、其他零件重疊。</p>
  </div>
  <div class="card">
    <h3>Solver</h3>
    <p>回溯搜尋，自動找出一組可過關的放置方式。</p>
  </div>
  <div class="card">
    <h3>關卡設計器</h3>
    <p>編輯棋盤與零件，並用 C++ solver 驗證是否可解。</p>
  </div>
</div>

---

## Solver 與關卡設計器

<div class="split">
  <div class="panel">
    <h3>Solver 做什麼</h3>
    <ol>
      <li>保留 fixed / blocked 格</li>
      <li>產生不重複旋轉</li>
      <li>大零件優先放</li>
      <li>超過 row / col 目標就剪枝</li>
    </ol>
  </div>
  <div class="panel">
    <h3>設計器怎麼避免無解</h3>
    <div class="flow" style="margin-top: 8px">
      <div class="node"><b>Designer</b><span>config text</span></div>
      <div class="arrow">→</div>
      <div class="node"><b>C++</b><span>parse + solve</span></div>
    </div>
    <p style="margin-top: 18px">有解才開放輸出與存檔。</p>
  </div>
</div>

---

## AI 協作與開發流程

<div class="two">
  <div class="panel">
    <h3>AI 加速的部分</h3>
    <ul>
      <li>Vue component 與 CSS</li>
      <li>Electron IPC 樣板</li>
      <li>跨語言資料結構整理</li>
      <li>文件與報告整理</li>
    </ul>
  </div>
  <div class="panel">
    <h3>人工把關的部分</h3>
    <ul>
      <li>C++ 必須負責核心規則</li>
      <li>前後端責任邊界</li>
      <li>實際操作測試與 demo 驗收</li>
      <li>不把規則搬到 TypeScript</li>
    </ul>
  </div>
</div>

---

## Demo 測試流程

<div class="three">
  <div class="card">
    <div class="big">1</div>
    <h3>載入測資</h3>
    <p><code>backend/tests/Example*.txt</code></p>
  </div>
  <div class="card">
    <div class="big">2</div>
    <h3>操作棋盤</h3>
    <p>拖曳、旋轉、合法 / 非法放置、重置。</p>
  </div>
  <div class="card">
    <div class="big">3</div>
    <h3>展示功能</h3>
    <p>提示、AI幫你放、過關判定。</p>
  </div>
</div>

<div class="flow">
  <div class="node"><b>Load</b><span>既有關卡</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>Play</b><span>拖曳旋轉</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>Auto Solve</b><span>solver 展示</span></div>
  <div class="arrow">→</div>
  <div class="node"><b>Design</b><span>自訂關卡</span></div>
</div>

---

<!-- _class: lead -->

<div class="split">
  <div>
    <div class="kicker">Takeaway</div>
    <h1>規則放後端，體驗交前端</h1>
  </div>
  <div class="panel">
    <ul>
      <li>C++ 集中管理遊戲規則與 solver</li>
      <li>Electron + Vue 專注桌面 UI 與互動</li>
      <li>關卡設計器讓專案不只是一個單關卡 demo</li>
      <li>AI 用來加速實作，但不取代架構判斷</li>
    </ul>
  </div>
</div>
