// Level data types — mirror the C++ config-file format described in project rule.pdf.
// Coordinates are (row, col), top-left = (0, 0).

export type ColorIndex = number

export interface PieceDef {
  id: number
  color: ColorIndex
  // shape[row][col] = 1 if the piece occupies that cell, 0 otherwise.
  // Dimensions M2 × N2.
  shape: number[][]
}

export interface FixedPiece {
  color: ColorIndex
  row: number
  col: number
}

export interface Level {
  colorCount: number
  rows: number
  cols: number
  // rowCounts[color][r] = required filled cells of that color in row r
  rowCounts: number[][]
  // colCounts[color][c] = required filled cells of that color in column c
  colCounts: number[][]
  // Per-color list of immovable single-cell pieces already on the board.
  fixedPieces: FixedPiece[]
  // Cells where no piece can be placed.
  blockedCells: { row: number; col: number }[]
  pieces: PieceDef[]
}
