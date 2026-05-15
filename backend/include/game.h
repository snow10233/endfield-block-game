#pragma once
#include <string>
#include <vector>

#include "board.h"
#include "file-loader.h"
#include "part.h"

struct PlaceResult {
  bool ok = false;
  std::string error;
  bool won = false;
};

class Game {
 private:
  FileLoader loader_;
  Board board_;
  std::vector<Part> parts_;
  bool loaded_ = false;

  // Apply level data (fixed pieces + blocked cells) to a freshly-resized board.
  void applyLevelToBoard();

  // Erase a piece's footprint from the board (used by remove/reset).
  void clearPieceCells(const Part& p);

  // Write a piece's footprint to the board (used by place/restore).
  void writePieceCells(const Part& p);

  // Check that the rotated piece fits at (row, col): no overlap, no
  // out-of-bounds, not on a blocked cell. ignorePieceId can be set to skip
  // collisions with the piece's own current cells (useful for in-place rotate).
  std::string canPlace(const Part& p, int row, int col, int rotation,
                       int ignorePieceId) const;

 public:
  bool load(const std::string& fileName);
  void reset();

  int getRows() const { return board_.getRows(); }
  int getCols() const { return board_.getCols(); }
  int getColorCount() const { return loader_.getData().colors; }

  const Board& getBoard() const { return board_; }
  const std::vector<Part>& getParts() const { return parts_; }
  const GameDataPacket& getLevelData() const { return loader_.getData(); }

  PlaceResult placePart(int pieceId, int row, int col, int rotation);
  bool removePart(int pieceId);

  bool isWon() const;

  // Current achieved counts (for hint display). [color][index]
  std::vector<std::vector<int>> getRowFilled() const;
  std::vector<std::vector<int>> getColFilled() const;
};
