#include "file-loader.h"

#include <fstream>
#include <sstream>

FileLoader::FileLoader(std::string _fileName) : fileName(_fileName) {}

void FileLoader::setFileName(std::string _fileName) { fileName = _fileName; }

bool FileLoader::load() {
  std::ifstream input(fileName, std::ios::in);

  if (input.fail()) {
    return false;
  }

  input >> datas.colors >> datas.row >> datas.col;

  for (int i = 0; i < datas.colors; ++i) {
    OneColorRule Needs;

    for (int j = 0; j < datas.row; ++j) {
      int temp;
      input >> temp;
      Needs.rowNeed.push_back(temp);
    }

    for (int j = 0; j < datas.col; ++j) {
      int temp;
      input >> temp;
      Needs.colNeed.push_back(temp);
    }

    int fixedsNum;
    input >> fixedsNum;

    for (int j = 0; j < fixedsNum; ++j) {
      int row, col;
      input >> row >> col;
      Needs.fixeds.push_back(Position(col, row));
    }

    datas.rules.push_back(Needs);
  }

  int nonColorFixedsNum;
  input >> nonColorFixedsNum;

  for (int i = 0; i < nonColorFixedsNum; ++i) {
    int row, col;
    input >> row >> col;
    datas.nonColorFixeds.push_back(Position(col, row));
  }

  int color, row, col;

  while (input >> color >> row >> col) {
    std::vector<std::vector<int>> shape;
    shape.assign(row, std::vector<int>(col, 0));

    for (int i = 0; i < row; ++i) {
      for (int j = 0; j < col; ++j) {
        input >> shape[i][j];
      }
    }

    datas.partDatas.push_back(Part(col, row, color, shape));
  }

  return true;
}
