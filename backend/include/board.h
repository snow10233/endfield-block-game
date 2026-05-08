#pragma once
#include <iostream>
#include <vector>

class Board {
  friend std::ostream& operator<<(std::ostream& os, const Board& b);

 private:
  int col, row;
  std::vector<std::vector<char>> board;

 public:
  Board() : col(-1), row(-1) {}

  Board(int _col, int _row, std::vector<std::vector<char>> _board)
      : col(_col), row(_row), board(_board) {}

  void setCol(int c) {
    if (c > 0) col = c;
  }

  void setRow(int r) {
    if (r > 0) row = r;
  }

  void setBoard(const std::vector<std::vector<char>>& b) { board = b; }

  bool setElement(int x, int y, char value) {
    if (x >= 0 && x < col && y >= 0 && y < row) {
      board[y][x] = value;
      return true;
    }
    return false;
  }

  char getElement(int x, int y) const {
    if (x >= 0 && x < col && y >= 0 && y < row) {
      return board[y][x];
    }
    return -1;  // Invalid position
  }

  int getRow() const { return row; }

  int getCol() const { return col; }
};

std::ostream& operator<<(std::ostream& os, const Board& b) {
  for (int i = 0; i < b.row; ++i) {
    for (int j = 0; j < b.col; ++j) {
      os << b.board[i][j] << ' ';
    }
    os << std::endl;
  }
  return os;
}
