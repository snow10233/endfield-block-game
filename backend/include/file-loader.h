#pragma once
#include <fstream>
#include <sstream>
#include <string>

#include "game-data-packet.h"
class FileLoader {
 private:
  std::string fileName;
  GameDataPacket datas;

 public:
  FileLoader(std::string _fileName = "");

  void setFileName(std::string _fileName);

  bool load();

  const std::string& getFileName() const { return fileName; }

  const GameDataPacket& getData() const { return datas; }
};
