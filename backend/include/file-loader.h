#pragma once
#include <fstream>
#include <istream>
#include <sstream>
#include <string>

#include "game-data-packet.h"
class FileLoader {
 private:
  std::string fileName;
  GameDataPacket datas;

  // Shared body used by both file and string loaders.
  bool parse(std::istream& input);

 public:
  FileLoader(std::string _fileName = "");

  void setFileName(std::string _fileName);

  bool load();
  bool loadFromString(const std::string& text);

  const std::string& getFileName() const { return fileName; }

  const GameDataPacket& getData() const { return datas; }
};
