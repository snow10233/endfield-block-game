#include "board.h"

void Board::resize(int rows, int cols) {
  rows_ = rows;
  cols_ = cols;
  cells_.assign(rows, std::vector<Cell>(cols));
}

bool Board::inBounds(int row, int col) const {
  return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

std::ostream& operator<<(std::ostream& os, const Board& b) {
  for (int r = 0; r < b.rows_; ++r) {
    for (int c = 0; c < b.cols_; ++c) {
      const Cell& cell = b.cells_[r][c];
      char ch = '_';
      switch (cell.state) {
        case CellState::Empty:
          ch = '_';
          break;
        case CellState::Blocked:
          ch = 'X';
          break;
        case CellState::Fixed:
          ch = '=';
          break;
        case CellState::Movable:
          ch = static_cast<char>('0' + cell.pieceId);
          break;
      }
      os << ch;
      if (c + 1 < b.cols_) os << ' ';
    }
    os << '\n';
  }
  return os;
}
