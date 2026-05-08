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
  Part(int _col, int _row, int _color, std::vector<std::vector<int>> _shape);

  void setCol(int c);

  void setRow(int r);

  void setMid(int x, int y);

  int getCol() const { return col; }

  int getRow() const { return row; }

  int getColor() const { return color; }

  const Position& getMid() const { return mid; }

  // void rotate();
};
