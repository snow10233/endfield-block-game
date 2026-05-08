#include "board.h"

std::ostream& operator<<(std::ostream& os, const Board& b) {
  for (int i = 0; i < b.row; ++i) {
    for (int j = 0; j < b.col; ++j) {
      os << b.board[i][j] << ' ';
    }
    os << std::endl;
  }
  return os;
}

Board::Board() : col(-1), row(-1) {}

Board::Board(int _col, int _row, std::vector<std::vector<char>> _board)
    : col(_col), row(_row), board(_board) {}

void Board::setCol(int c) {
  if (c > 0) col = c;
}

void Board::setRow(int r) {
  if (r > 0) row = r;
}

void Board::setBoard(const std::vector<std::vector<char>>& b) { board = b; }

bool Board::setElement(int x, int y, char value) {
  if (x >= 0 && x < col && y >= 0 && y < row) {
    board[y][x] = value;
    return true;
  }
  return false;
}

char Board::getElement(int x, int y) const {
  if (x >= 0 && x < col && y >= 0 && y < row) {
    return board[y][x];
  }
  return -1;  // Invalid position
}
