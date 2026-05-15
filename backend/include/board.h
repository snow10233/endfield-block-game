#pragma once
#include <iostream>
#include <vector>

#include "cell.h"

// Owns the M×N grid of `Cell`s. Pure state container — placement rules and
// win-check live in Game.
class Board {
  friend std::ostream& operator<<(std::ostream& os, const Board& b);

 private:
  int rows_ = 0;
  int cols_ = 0;
  std::vector<std::vector<Cell>> cells_;

 public:
  Board() = default;

  void resize(int rows, int cols);

  int getRows() const { return rows_; }
  int getCols() const { return cols_; }

  bool inBounds(int row, int col) const;

  const Cell& at(int row, int col) const { return cells_[row][col]; }
  Cell& at(int row, int col) { return cells_[row][col]; }
};
