#pragma once

enum class CellState {
  Empty,    // can place a piece here
  Blocked,  // 不可放置 — config-file 'X'
  Fixed,    // pre-placed immovable colored cell — config-file '='
  Movable,  // currently occupied by a movable piece
};

struct Cell {
  CellState state = CellState::Empty;
  int color = -1;    // valid for Fixed / Movable
  int pieceId = -1;  // valid for Movable; -1 otherwise
};
