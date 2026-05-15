#include "game.h"

#include <algorithm>
#include <iostream>

bool Game::load(const std::string& fileName) {
  loader_.setFileName(fileName);
  if (!loader_.load()) return false;
  applyLoaded();
  return true;
}

bool Game::loadFromString(const std::string& text) {
  if (!loader_.loadFromString(text)) return false;
  applyLoaded();
  return true;
}

void Game::applyLoaded() {
  const GameDataPacket& data = loader_.getData();
  board_.resize(data.row, data.col);
  parts_.clear();
  for (const Part& src : data.partDatas) {
    parts_.push_back(src);  // fresh copy, un-placed
  }
  applyLevelToBoard();
  loaded_ = true;
}

void Game::reset() {
  if (!loaded_) return;
  for (Part& p : parts_) {
    p.unplace();
    p.setRotation(0);
  }
  // re-initialize cells then write fixed/blocked from level data
  board_.resize(board_.getRows(), board_.getCols());
  applyLevelToBoard();
}

void Game::applyLevelToBoard() {
  const GameDataPacket& data = loader_.getData();
  for (int color = 0; color < static_cast<int>(data.rules.size()); ++color) {
    for (const Position& p : data.rules[color].fixeds) {
      Cell& c = board_.at(p.y, p.x);
      c.state = CellState::Fixed;
      c.color = color;
      c.pieceId = -1;
    }
  }
  for (const Position& p : data.nonColorFixeds) {
    Cell& c = board_.at(p.y, p.x);
    c.state = CellState::Blocked;
  }
}

std::string Game::canPlace(const Part& p, int row, int col, int rotation,
                           int ignorePieceId) const {
  Part probe = p;
  probe.setRotation(rotation);
  const auto shape = probe.getCurrentShape();
  const int sr = static_cast<int>(shape.size());
  const int sc = sr == 0 ? 0 : static_cast<int>(shape[0].size());

  for (int r = 0; r < sr; ++r) {
    for (int c = 0; c < sc; ++c) {
      if (shape[r][c] != 1) continue;
      int br = row + r;
      int bc = col + c;
      if (!board_.inBounds(br, bc)) return "out of bounds";
      const Cell& cell = board_.at(br, bc);
      if (cell.state == CellState::Blocked) return "cell is blocked";
      if (cell.state == CellState::Fixed) return "cell already occupied";
      if (cell.state == CellState::Movable && cell.pieceId != ignorePieceId)
        return "cell already occupied";
    }
  }
  return {};
}

void Game::clearPieceCells(const Part& p) {
  const auto shape = p.getCurrentShape();
  for (int r = 0; r < static_cast<int>(shape.size()); ++r) {
    for (int c = 0; c < static_cast<int>(shape[r].size()); ++c) {
      if (shape[r][c] != 1) continue;
      int br = p.getPlacedRow() + r;
      int bc = p.getPlacedCol() + c;
      if (!board_.inBounds(br, bc)) continue;
      Cell& cell = board_.at(br, bc);
      if (cell.state == CellState::Movable && cell.pieceId == p.getId()) {
        cell.state = CellState::Empty;
        cell.color = -1;
        cell.pieceId = -1;
      }
    }
  }
}

void Game::writePieceCells(const Part& p) {
  const auto shape = p.getCurrentShape();
  for (int r = 0; r < static_cast<int>(shape.size()); ++r) {
    for (int c = 0; c < static_cast<int>(shape[r].size()); ++c) {
      if (shape[r][c] != 1) continue;
      int br = p.getPlacedRow() + r;
      int bc = p.getPlacedCol() + c;
      Cell& cell = board_.at(br, bc);
      cell.state = CellState::Movable;
      cell.color = p.getColor();
      cell.pieceId = p.getId();
    }
  }
}

PlaceResult Game::placePart(int pieceId, int row, int col, int rotation) {
  PlaceResult res;
  if (pieceId < 0 || pieceId >= static_cast<int>(parts_.size())) {
    res.error = "invalid pieceId";
    return res;
  }
  Part& p = parts_[pieceId];

  // If the piece is currently placed, lift it off first so it doesn't collide
  // with itself.
  bool wasPlaced = p.isPlaced();
  if (wasPlaced) clearPieceCells(p);

  std::string err = canPlace(p, row, col, rotation, pieceId);
  if (!err.empty()) {
    // restore the piece's previous placement if we lifted it
    if (wasPlaced) writePieceCells(p);
    res.error = err;
    return res;
  }

  p.setRotation(rotation);
  p.place(row, col);
  writePieceCells(p);

  res.ok = true;
  res.won = isWon();
  return res;
}

bool Game::removePart(int pieceId) {
  if (pieceId < 0 || pieceId >= static_cast<int>(parts_.size())) return false;
  Part& p = parts_[pieceId];
  if (!p.isPlaced()) return false;
  clearPieceCells(p);
  p.unplace();
  return true;
}

bool Game::isWon() const {
  const GameDataPacket& data = loader_.getData();
  for (int color = 0; color < data.colors; ++color) {
    const OneColorRule& rule = data.rules[color];
    // rows
    for (int r = 0; r < data.row; ++r) {
      int filled = 0;
      for (int c = 0; c < data.col; ++c) {
        const Cell& cell = board_.at(r, c);
        bool isThisColor =
            (cell.state == CellState::Fixed || cell.state == CellState::Movable) &&
            cell.color == color;
        if (isThisColor) ++filled;
      }
      if (filled != rule.rowNeed[r]) return false;
    }
    // cols
    for (int c = 0; c < data.col; ++c) {
      int filled = 0;
      for (int r = 0; r < data.row; ++r) {
        const Cell& cell = board_.at(r, c);
        bool isThisColor =
            (cell.state == CellState::Fixed || cell.state == CellState::Movable) &&
            cell.color == color;
        if (isThisColor) ++filled;
      }
      if (filled != rule.colNeed[c]) return false;
    }
  }
  return true;
}

std::vector<std::vector<int>> Game::getRowFilled() const {
  const GameDataPacket& data = loader_.getData();
  std::vector<std::vector<int>> out(data.colors, std::vector<int>(data.row, 0));
  for (int r = 0; r < data.row; ++r) {
    for (int c = 0; c < data.col; ++c) {
      const Cell& cell = board_.at(r, c);
      if (cell.state == CellState::Fixed || cell.state == CellState::Movable) {
        if (cell.color >= 0 && cell.color < data.colors) out[cell.color][r]++;
      }
    }
  }
  return out;
}

std::vector<std::vector<int>> Game::getColFilled() const {
  const GameDataPacket& data = loader_.getData();
  std::vector<std::vector<int>> out(data.colors, std::vector<int>(data.col, 0));
  for (int r = 0; r < data.row; ++r) {
    for (int c = 0; c < data.col; ++c) {
      const Cell& cell = board_.at(r, c);
      if (cell.state == CellState::Fixed || cell.state == CellState::Movable) {
        if (cell.color >= 0 && cell.color < data.colors) out[cell.color][c]++;
      }
    }
  }
  return out;
}
