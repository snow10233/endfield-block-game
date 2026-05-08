#pragma once
#include "board.h"
#include "file-loader.h"
#include "part.h"

class Game {
 private:
  FileLoader loader;
  Board gameBoard;
  std::vector<Part> parts;

 public:
  Game();

  void init(std::string _fileName);

  void start();

  void reset();

  void exit();
};
