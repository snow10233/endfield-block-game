#include "part.h"

namespace {
// Rotate a 2D matrix 90° CW once.
std::vector<std::vector<int>> rotateOnceCW(
    const std::vector<std::vector<int>>& m) {
  const int r = static_cast<int>(m.size());
  const int c = r == 0 ? 0 : static_cast<int>(m[0].size());
  std::vector<std::vector<int>> out(c, std::vector<int>(r, 0));
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      // (i, j) -> (j, r - 1 - i)
      out[j][r - 1 - i] = m[i][j];
    }
  }
  return out;
}

// Map a coordinate in the base shape through `n` 90° CW rotations.
// Returns the coordinate in the n-rotated shape's local space.
Position rotatePos(Position p, int baseRows, int baseCols, int n) {
  int x = p.x, y = p.y;          // x = col, y = row (per Position layout)
  int rows = baseRows, cols = baseCols;
  for (int k = 0; k < n; ++k) {
    // (row=y, col=x) -> (row=x, col=rows-1-y) in new shape with swapped dims
    int nx = rows - 1 - y;
    int ny = x;
    x = nx;
    y = ny;
    std::swap(rows, cols);
  }
  return Position(x, y);
}
}  // namespace

Part::Part(int id, int color, std::vector<std::vector<int>> baseShape)
    : id_(id), color_(color), baseShape_(std::move(baseShape)) {
  // pick the first filled cell as the mid handle (row-major scan)
  for (int r = 0; r < static_cast<int>(baseShape_.size()); ++r) {
    for (int c = 0; c < static_cast<int>(baseShape_[r].size()); ++c) {
      if (baseShape_[r][c] == 1) {
        baseMid_ = Position(c, r);
        return;
      }
    }
  }
}

std::vector<std::vector<int>> Part::getCurrentShape() const {
  std::vector<std::vector<int>> s = baseShape_;
  for (int k = 0; k < rotation_; ++k) s = rotateOnceCW(s);
  return s;
}

int Part::getCurrentRows() const {
  if (rotation_ % 2 == 0) return static_cast<int>(baseShape_.size());
  return baseShape_.empty() ? 0 : static_cast<int>(baseShape_[0].size());
}

int Part::getCurrentCols() const {
  if (rotation_ % 2 == 0)
    return baseShape_.empty() ? 0 : static_cast<int>(baseShape_[0].size());
  return static_cast<int>(baseShape_.size());
}

Position Part::getCurrentMid() const {
  const int baseRows = static_cast<int>(baseShape_.size());
  const int baseCols =
      baseShape_.empty() ? 0 : static_cast<int>(baseShape_[0].size());
  return rotatePos(baseMid_, baseRows, baseCols, rotation_);
}

void Part::setRotation(int r) { rotation_ = ((r % 4) + 4) % 4; }
void Part::rotateCW() { rotation_ = (rotation_ + 1) % 4; }

void Part::place(int row, int col) {
  placed_ = true;
  placedRow_ = row;
  placedCol_ = col;
}

void Part::unplace() {
  placed_ = false;
  placedRow_ = -1;
  placedCol_ = -1;
}
