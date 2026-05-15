#include "solver.h"

#include <algorithm>

namespace {

std::vector<std::vector<int>> rotateOnceCW(
    const std::vector<std::vector<int>>& m) {
  const int r = static_cast<int>(m.size());
  const int c = r == 0 ? 0 : static_cast<int>(m[0].size());
  std::vector<std::vector<int>> out(c, std::vector<int>(r, 0));
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      out[j][r - 1 - i] = m[i][j];
    }
  }
  return out;
}

// Generate distinct rotations of a base shape. Returns pairs of
// (rotation-index 0..3, shape). Symmetric rotations are de-duplicated to
// keep the search space small.
std::vector<std::pair<int, std::vector<std::vector<int>>>> distinctRotations(
    const std::vector<std::vector<int>>& base) {
  std::vector<std::pair<int, std::vector<std::vector<int>>>> out;
  auto s = base;
  for (int k = 0; k < 4; ++k) {
    bool seen = false;
    for (const auto& [_, prev] : out) {
      if (prev == s) {
        seen = true;
        break;
      }
    }
    if (!seen) out.push_back({k, s});
    s = rotateOnceCW(s);
  }
  return out;
}

// Count filled cells in a shape — used to compare piece size.
int filledCells(const std::vector<std::vector<int>>& shape) {
  int n = 0;
  for (const auto& row : shape)
    for (int v : row)
      if (v == 1) ++n;
  return n;
}

}  // namespace

bool Solver::solve(const Game& game, std::vector<SolverPlacement>& out) {
  const GameDataPacket& data = game.getLevelData();
  rows_ = data.row;
  cols_ = data.col;
  colors_ = data.colors;

  // Initialize cell grid with fixed pieces / blocked cells from the level
  // (movable pieces' current placements are ignored — solve from scratch).
  cell_.assign(rows_, std::vector<int>(cols_, -1));
  for (int color = 0; color < colors_; ++color) {
    for (const Position& p : data.rules[color].fixeds) {
      cell_[p.y][p.x] = color;
    }
  }
  for (const Position& p : data.nonColorFixeds) {
    cell_[p.y][p.x] = -2;
  }

  rowTarget_.assign(colors_, std::vector<int>(rows_, 0));
  colTarget_.assign(colors_, std::vector<int>(cols_, 0));
  rowFilled_.assign(colors_, std::vector<int>(rows_, 0));
  colFilled_.assign(colors_, std::vector<int>(cols_, 0));
  for (int color = 0; color < colors_; ++color) {
    rowTarget_[color] = data.rules[color].rowNeed;
    colTarget_[color] = data.rules[color].colNeed;
    // seed filled counts from fixed cells already on the board
    for (int r = 0; r < rows_; ++r) {
      for (int c = 0; c < cols_; ++c) {
        if (cell_[r][c] == color) {
          rowFilled_[color][r]++;
          colFilled_[color][c]++;
        }
      }
    }
  }

  // Build piece list. Sort by piece size descending: bigger pieces are more
  // constraining so trying them first prunes faster.
  parts_.clear();
  parts_.reserve(data.partDatas.size());
  for (const Part& p : data.partDatas) {
    PartInfo info;
    info.id = p.getId();
    info.color = p.getColor();
    info.shapes = distinctRotations(p.getBaseShape());
    parts_.push_back(std::move(info));
  }
  std::vector<int> order(parts_.size());
  for (int i = 0; i < static_cast<int>(order.size()); ++i) order[i] = i;
  std::sort(order.begin(), order.end(), [&](int a, int b) {
    return filledCells(parts_[a].shapes[0].second) >
           filledCells(parts_[b].shapes[0].second);
  });
  std::vector<PartInfo> sorted;
  sorted.reserve(parts_.size());
  for (int i : order) sorted.push_back(parts_[i]);
  parts_ = std::move(sorted);

  partial_.clear();
  partial_.reserve(parts_.size());

  if (!recurse(0)) return false;

  // partial_ is in solving order, not piece-id order. Re-sort by pieceId.
  std::sort(partial_.begin(), partial_.end(),
            [](const SolverPlacement& a, const SolverPlacement& b) {
              return a.pieceId < b.pieceId;
            });
  out = partial_;
  return true;
}

bool Solver::canPlace(const std::vector<std::vector<int>>& shape, int row,
                      int col, int color) const {
  const int sr = static_cast<int>(shape.size());
  const int sc = sr == 0 ? 0 : static_cast<int>(shape[0].size());
  for (int r = 0; r < sr; ++r) {
    for (int c = 0; c < sc; ++c) {
      if (shape[r][c] != 1) continue;
      int br = row + r;
      int bc = col + c;
      if (br < 0 || br >= rows_ || bc < 0 || bc >= cols_) return false;
      if (cell_[br][bc] != -1) return false;
      // Pre-check that placing here doesn't exceed targets
      if (rowFilled_[color][br] + 1 > rowTarget_[color][br]) return false;
      if (colFilled_[color][bc] + 1 > colTarget_[color][bc]) return false;
    }
  }
  return true;
}

void Solver::doPlace(const std::vector<std::vector<int>>& shape, int row,
                     int col, int color) {
  for (int r = 0; r < static_cast<int>(shape.size()); ++r) {
    for (int c = 0; c < static_cast<int>(shape[r].size()); ++c) {
      if (shape[r][c] != 1) continue;
      int br = row + r;
      int bc = col + c;
      cell_[br][bc] = color;
      rowFilled_[color][br]++;
      colFilled_[color][bc]++;
    }
  }
}

void Solver::undoPlace(const std::vector<std::vector<int>>& shape, int row,
                       int col, int color) {
  for (int r = 0; r < static_cast<int>(shape.size()); ++r) {
    for (int c = 0; c < static_cast<int>(shape[r].size()); ++c) {
      if (shape[r][c] != 1) continue;
      int br = row + r;
      int bc = col + c;
      cell_[br][bc] = -1;
      rowFilled_[color][br]--;
      colFilled_[color][bc]--;
    }
  }
}

bool Solver::meetsAllTargets() const {
  for (int color = 0; color < colors_; ++color) {
    for (int r = 0; r < rows_; ++r)
      if (rowFilled_[color][r] != rowTarget_[color][r]) return false;
    for (int c = 0; c < cols_; ++c)
      if (colFilled_[color][c] != colTarget_[color][c]) return false;
  }
  return true;
}

bool Solver::recurse(int pieceIdx) {
  if (pieceIdx == static_cast<int>(parts_.size())) {
    return meetsAllTargets();
  }
  const PartInfo& info = parts_[pieceIdx];
  for (const auto& [rot, shape] : info.shapes) {
    const int sr = static_cast<int>(shape.size());
    const int sc = sr == 0 ? 0 : static_cast<int>(shape[0].size());
    for (int row = 0; row + sr <= rows_; ++row) {
      for (int col = 0; col + sc <= cols_; ++col) {
        if (!canPlace(shape, row, col, info.color)) continue;
        doPlace(shape, row, col, info.color);
        partial_.push_back({info.id, row, col, rot});
        if (recurse(pieceIdx + 1)) return true;
        partial_.pop_back();
        undoPlace(shape, row, col, info.color);
      }
    }
  }
  return false;
}
