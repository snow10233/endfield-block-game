#pragma once
#include <iostream>
#include <vector>

#include "postion.h"

class Part {
 private:
  int col, row, color;
  Position mid;
  std::vector<std::vector<int>> shape;

 public:
  Part(int _col, int _row, int _color, std::vector<std::vector<int>> _shape)
      : col(_col), row(_row), color(_color), shape(_shape) {}

  void setCol(int c) {
    if (c > 0) col = c;
  }

  void setRow(int r) {
    if (r > 0) row = r;
  }

  void setMid(int x, int y) { mid.x = x, mid.y = y; }

  int getCol() const { return col; }

  int getRow() const { return row; }

  int getColor() const { return color; }

  const Position& getMid() const { return mid; }

  // void rotate();
};
