#include <iostream>
#include <string>

#include "game.h"
#include "json.h"

namespace {

void writeState(JsonWriter& w, const Game& game, bool loaded) {
  w.beginObject();
  w.key("loaded");
  w.valueBool(loaded);
  if (!loaded) {
    w.endObject();
    return;
  }
  const GameDataPacket& data = game.getLevelData();
  w.key("colorCount");
  w.valueInt(data.colors);
  w.key("rows");
  w.valueInt(data.row);
  w.key("cols");
  w.valueInt(data.col);

  // rowCounts[color][row]
  w.key("rowCounts");
  w.beginArray();
  for (int c = 0; c < data.colors; ++c) {
    w.beginArray();
    for (int v : data.rules[c].rowNeed) w.valueInt(v);
    w.endArray();
  }
  w.endArray();

  w.key("colCounts");
  w.beginArray();
  for (int c = 0; c < data.colors; ++c) {
    w.beginArray();
    for (int v : data.rules[c].colNeed) w.valueInt(v);
    w.endArray();
  }
  w.endArray();

  // currently-achieved counts
  auto rowFilled = game.getRowFilled();
  w.key("rowFilled");
  w.beginArray();
  for (const auto& row : rowFilled) {
    w.beginArray();
    for (int v : row) w.valueInt(v);
    w.endArray();
  }
  w.endArray();

  auto colFilled = game.getColFilled();
  w.key("colFilled");
  w.beginArray();
  for (const auto& row : colFilled) {
    w.beginArray();
    for (int v : row) w.valueInt(v);
    w.endArray();
  }
  w.endArray();

  // blocked cells
  w.key("blockedCells");
  w.beginArray();
  for (const Position& p : data.nonColorFixeds) {
    w.beginObject();
    w.key("row");
    w.valueInt(p.y);
    w.key("col");
    w.valueInt(p.x);
    w.endObject();
  }
  w.endArray();

  // fixed pieces (immovable, with color)
  w.key("fixedPieces");
  w.beginArray();
  for (int color = 0; color < data.colors; ++color) {
    for (const Position& p : data.rules[color].fixeds) {
      w.beginObject();
      w.key("row");
      w.valueInt(p.y);
      w.key("col");
      w.valueInt(p.x);
      w.key("color");
      w.valueInt(color);
      w.endObject();
    }
  }
  w.endArray();

  // pieces (movable)
  w.key("pieces");
  w.beginArray();
  for (const Part& p : game.getParts()) {
    w.beginObject();
    w.key("id");
    w.valueInt(p.getId());
    w.key("color");
    w.valueInt(p.getColor());
    w.key("rotation");
    w.valueInt(p.getRotation());
    w.key("placed");
    w.valueBool(p.isPlaced());
    w.key("row");
    w.valueInt(p.getPlacedRow());
    w.key("col");
    w.valueInt(p.getPlacedCol());
    w.key("baseShape");
    w.beginArray();
    for (const auto& row : p.getBaseShape()) {
      w.beginArray();
      for (int v : row) w.valueInt(v);
      w.endArray();
    }
    w.endArray();
    w.endObject();
  }
  w.endArray();

  w.key("won");
  w.valueBool(game.isWon());

  w.endObject();
}

// Compose a final response line by splicing id + payload object.
std::string buildResponse(int id, const std::string& payloadJsonObject) {
  std::ostringstream out;
  out << "{\"id\":" << id;
  if (!payloadJsonObject.empty()) {
    // Splice the inner fields of payload object into ours by stripping
    // the leading '{'.
    if (payloadJsonObject.size() >= 2 && payloadJsonObject.front() == '{' &&
        payloadJsonObject.back() == '}') {
      std::string inner = payloadJsonObject.substr(1, payloadJsonObject.size() - 2);
      if (!inner.empty()) {
        out << "," << inner;
      }
    } else {
      // Not an object, attach as result field
      out << ",\"result\":" << payloadJsonObject;
    }
  }
  out << '}';
  return out.str();
}

}  // namespace

int main() {
  Game game;
  bool loaded = false;
  std::string line;

  // line-buffered I/O; flush after every response
  std::cout.setf(std::ios::unitbuf);

  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;

    int id = json_util::getInt(line, "id", 0);
    std::string op = json_util::getStr(line, "op");

    JsonWriter payload;

    if (op == "load") {
      std::string path = json_util::getStr(line, "path");
      bool ok = game.load(path);
      loaded = ok;
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(ok);
      if (!ok) {
        payload.key("error");
        payload.valueStr("failed to load file: " + path);
      }
      payload.endObject();
    } else if (op == "state") {
      writeState(payload, game, loaded);
    } else if (op == "place") {
      int pieceId = json_util::getInt(line, "pieceId", -1);
      int row = json_util::getInt(line, "row", -1);
      int col = json_util::getInt(line, "col", -1);
      int rot = json_util::getInt(line, "rot", 0);
      PlaceResult r = game.placePart(pieceId, row, col, rot);
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(r.ok);
      if (!r.ok) {
        payload.key("error");
        payload.valueStr(r.error);
      }
      payload.key("won");
      payload.valueBool(r.won);
      payload.endObject();
    } else if (op == "remove") {
      int pieceId = json_util::getInt(line, "pieceId", -1);
      bool ok = game.removePart(pieceId);
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(ok);
      payload.endObject();
    } else if (op == "reset") {
      game.reset();
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(true);
      payload.endObject();
    } else if (op == "quit") {
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(true);
      payload.endObject();
      std::cout << buildResponse(id, payload.str()) << '\n';
      break;
    } else {
      payload.beginObject();
      payload.key("ok");
      payload.valueBool(false);
      payload.key("error");
      payload.valueStr("unknown op: " + op);
      payload.endObject();
    }

    std::cout << buildResponse(id, payload.str()) << '\n';
  }

  return 0;
}
