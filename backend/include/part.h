#pragma once
#include <vector>

#include "postion.h"

// A movable piece. Rotation is in quarter-turns clockwise (0..3).
// `baseShape` is the un-rotated shape from the config file.
// Placement state: when `placed`, the rotated shape's top-left is at
// (placedRow, placedCol) on the board.
class Part {
 private:
  int id_;
  int color_;
  std::vector<std::vector<int>> baseShape_;
  Position baseMid_;  // in base-shape local coords; always a filled cell
  int rotation_ = 0;  // 0..3 (each step = 90° CW)
  bool placed_ = false;
  int placedRow_ = -1;
  int placedCol_ = -1;

 public:
  Part(int id, int color, std::vector<std::vector<int>> baseShape);

  int getId() const { return id_; }
  int getColor() const { return color_; }
  int getRotation() const { return rotation_; }

  bool isPlaced() const { return placed_; }
  int getPlacedRow() const { return placedRow_; }
  int getPlacedCol() const { return placedCol_; }

  const std::vector<std::vector<int>>& getBaseShape() const { return baseShape_; }

  // Shape after applying current rotation. Returned by value because the
  // dimensions can swap on odd rotations.
  std::vector<std::vector<int>> getCurrentShape() const;

  // Dimensions of the currently-rotated shape's bounding box.
  int getCurrentRows() const;
  int getCurrentCols() const;

  // Mid (rotation/move handle) in the currently-rotated shape's local coords.
  // Guaranteed to land on a filled cell of the current shape.
  Position getCurrentMid() const;

  void setRotation(int r);
  void rotateCW();  // advance rotation by 1 step
  void place(int row, int col);
  void unplace();
};
