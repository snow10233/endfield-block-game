#include "game.h"

#include <iostream>

Game::Game() {}

void Game::init(std::string _fileName) { loader.setFileName(_fileName); }

void Game::start() {
  if (!loader.load()) {
    std::cerr << "Failed to load game data from file: " << loader.getFileName()
              << std::endl;
    return;
  }

  const GameDataPacket& data = loader.getData();

  gameBoard.setRow(data.row);
  gameBoard.setCol(data.col);

  std::vector<std::vector<char>> boardDataInit(
      data.row, std::vector<char>(data.col, '_'));

  for (int i = 0; i < data.rules.size(); ++i) {
    const OneColorRule& rule = data.rules[i];
    char colorChar = 'A' + i;  // Assign a character for the color

    for (const Position& pos : rule.fixeds) {
      boardDataInit[pos.y][pos.x] = colorChar;
    }
  }

  for (const Position& pos : data.nonColorFixeds) {
    boardDataInit[pos.y][pos.x] = 'X';
  }

  gameBoard.setBoard(boardDataInit);

  std::cout << gameBoard << std::endl;
}

void Game::reset() {}

void Game::exit() {}
