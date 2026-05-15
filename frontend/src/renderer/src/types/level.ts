export type ColorIndex = number

export interface BlockedCell {
  row: number
  col: number
}

export interface FixedPiece {
  row: number
  col: number
  color: ColorIndex
}

export interface MovablePiece {
  id: number
  color: ColorIndex
  baseShape: number[][] // shape[r][c] = 1 if filled
  rotation: number // 0..3, each step 90° CW
  placed: boolean
  row: number // top-left of rotated bounding box; -1 if not placed
  col: number
}

export interface GameState {
  loaded: boolean
  colorCount: number
  rows: number
  cols: number
  // [color][row|col] = required count
  rowCounts: number[][]
  colCounts: number[][]
  // [color][row|col] = currently achieved count
  rowFilled: number[][]
  colFilled: number[][]
  blockedCells: BlockedCell[]
  fixedPieces: FixedPiece[]
  pieces: MovablePiece[]
  won: boolean
}

export interface PlaceResponse {
  id: number
  ok: boolean
  won: boolean
  error?: string
}

export interface SimpleOk {
  id: number
  ok: boolean
  error?: string
}
