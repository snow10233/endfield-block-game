# Screenshot Plan

Capture these images manually after building and running the app. Save them
under `docs/assets/screenshots/` so the README links can be enabled without
moving files later.

## Required Shots

| File | Screen | What to show |
| --- | --- | --- |
| `docs/assets/screenshots/start-screen.png` | Start screen | Title, load-level action, level-designer action. |
| `docs/assets/screenshots/game-board.png` | Active puzzle | Board, row/column counters, side piece panel, and top controls. |
| `docs/assets/screenshots/drag-preview.png` | Drag interaction | A piece mid-drag with green/red placement preview visible. |
| `docs/assets/screenshots/solver-hint.png` | Solver/hint | Hint overlay or auto-solved board after the 30-second unlock. |
| `docs/assets/screenshots/level-designer.png` | Level designer | Board editor, piece editor, validation status, export/save controls. |

## Capture Guidance

- Use the real Endfield Block Game runtime, not screenshots from another game.
- Prefer a 16:9 crop such as 1600x900 or 1920x1080 for GitHub readability.
- Keep the app window large enough that labels and board cells are readable.
- Capture one normal puzzle and one designer view; those two are the most useful
  portfolio images.
- After saving the files, add normal Markdown image embeds to the README:

```markdown
![Start screen](docs/assets/screenshots/start-screen.png)
![Game board](docs/assets/screenshots/game-board.png)
![Level designer](docs/assets/screenshots/level-designer.png)
```

