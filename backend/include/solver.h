#pragma once
#include <vector>

#include "game.h"

struct SolverPlacement {
  int pieceId;
  int row;
  int col;
  int rotation;
};

// Backtracking solver. Solves from the initial level state — any movable
// pieces currently placed on the board are ignored (treated as un-placed).
// Fixed pieces and blocked cells are respected.
class Solver {
 public:
  // Returns true and fills `out` (in piece-ID order) when a placement
  // satisfies every (row, color) and (col, color) target count.
  bool solve(const Game& game, std::vector<SolverPlacement>& out);

 private:
  int rows_ = 0;
  int cols_ = 0;
  int colors_ = 0;

  // cell color: -1 empty, -2 blocked, else color id (0..colors-1)
  std::vector<std::vector<int>> cell_;
  std::vector<std::vector<int>> rowFilled_;  // [color][row]
  std::vector<std::vector<int>> colFilled_;  // [color][col]
  std::vector<std::vector<int>> rowTarget_;  // [color][row]
  std::vector<std::vector<int>> colTarget_;  // [color][col]

  struct PartInfo {
    int id;
    int color;
    // De-duplicated rotated shapes (rotation -> shape).
    // We keep mapping from rotation-index used by `place` (the actual quarter
    // turn) so the caller gets the same rotation semantics as the runtime
    // Part class.
    std::vector<std::pair<int, std::vector<std::vector<int>>>> shapes;
  };
  std::vector<PartInfo> parts_;
  std::vector<SolverPlacement> partial_;

  bool recurse(int pieceIdx);
  bool canPlace(const std::vector<std::vector<int>>& shape, int row, int col,
                int color) const;
  void doPlace(const std::vector<std::vector<int>>& shape, int row, int col,
               int color);
  void undoPlace(const std::vector<std::vector<int>>& shape, int row, int col,
                 int color);
  bool exceedsAnyTarget() const;
  bool meetsAllTargets() const;
};
