#pragma once
#include "board.h"
#include "part.h"
#include "postion.h"

struct OneColorRule {
  std::vector<int> rowNeed, colNeed;
  std::vector<Position> fixeds;
};

struct GameDataPacket {
  int colors, row, col;
  std::vector<OneColorRule> rules;
  std::vector<Position> nonColorFixeds;
  std::vector<Part> partDatas;
};
