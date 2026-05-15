import type { Level } from '../types/level'

// Mirrors backend/tests/Example1.txt
//   1 3 3
//   3 2 1
//   3 2 1
//   0
//   0
//   0 2 2
//   1 1
//   1 0
//   0 1 3
//   1 1 1
export const example1: Level = {
  colorCount: 1,
  rows: 3,
  cols: 3,
  rowCounts: [[3, 2, 1]],
  colCounts: [[3, 2, 1]],
  fixedPieces: [],
  blockedCells: [],
  pieces: [
    {
      id: 0,
      color: 0,
      shape: [
        [1, 1],
        [1, 0]
      ]
    },
    {
      id: 1,
      color: 0,
      shape: [[1, 1, 1]]
    }
  ]
}
