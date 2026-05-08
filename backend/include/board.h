#pragma once
#include <iostream>
#include <vector>

class Board {
  friend std::ostream& operator<<(std::ostream& os, const Board& b);

 private:
  int col, row;
  std::vector<std::vector<char>> board;

 public:
  Board();

  Board(int _col, int _row, std::vector<std::vector<char>> _board);

  void setCol(int c);

  void setRow(int r);

  void setBoard(const std::vector<std::vector<char>>& b);

  bool setElement(int x, int y, char value);

  char getElement(int x, int y) const;

  int getRow() const { return row; }

  int getCol() const { return col; }
};
