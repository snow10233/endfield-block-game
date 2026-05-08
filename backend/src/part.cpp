#include "part.h"

Part::Part(int _col, int _row, int _color, std::vector<std::vector<int>> _shape)
    : col(_col), row(_row), color(_color), shape(_shape) {}

void Part::setCol(int c) {
  if (c > 0) col = c;
}

void Part::setRow(int r) {
  if (r > 0) row = r;
}

void Part::setMid(int x, int y) { mid.x = x, mid.y = y; }
