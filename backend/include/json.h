#pragma once
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

// Tiny JSON helpers for stdio IPC. We only emit/parse shallow JSON; nested
// structures are produced via raw stream methods. Not a general-purpose lib.

class JsonWriter {
  std::ostringstream out_;
  // Tracks whether the next value/key needs a leading comma. Stack matches
  // open object/array depth.
  std::vector<bool> first_;

  void prefix() {
    if (!first_.empty()) {
      if (!first_.back())
        out_ << ',';
      else
        first_.back() = false;
    }
  }

 public:
  void beginObject() {
    prefix();
    out_ << '{';
    first_.push_back(true);
  }
  void endObject() {
    out_ << '}';
    if (!first_.empty()) first_.pop_back();
  }
  void beginArray() {
    prefix();
    out_ << '[';
    first_.push_back(true);
  }
  void endArray() {
    out_ << ']';
    if (!first_.empty()) first_.pop_back();
  }

  void key(const std::string& k) {
    prefix();
    writeString(k);
    out_ << ':';
    // value after key shouldn't get a leading comma — push a sentinel that
    // resets after one value
    first_.push_back(true);
    pendingValue_ = true;
  }

  void valueInt(long long v) {
    if (!pendingValue_) prefix();
    out_ << v;
    afterValue();
  }
  void valueBool(bool v) {
    if (!pendingValue_) prefix();
    out_ << (v ? "true" : "false");
    afterValue();
  }
  void valueStr(const std::string& s) {
    if (!pendingValue_) prefix();
    writeString(s);
    afterValue();
  }
  void valueNull() {
    if (!pendingValue_) prefix();
    out_ << "null";
    afterValue();
  }

  std::string str() const { return out_.str(); }

 private:
  bool pendingValue_ = false;

  void afterValue() {
    if (pendingValue_) {
      pendingValue_ = false;
      if (!first_.empty()) first_.pop_back();
    }
  }

  void writeString(const std::string& s) {
    out_ << '"';
    for (char c : s) {
      switch (c) {
        case '"':
          out_ << "\\\"";
          break;
        case '\\':
          out_ << "\\\\";
          break;
        case '\n':
          out_ << "\\n";
          break;
        case '\r':
          out_ << "\\r";
          break;
        case '\t':
          out_ << "\\t";
          break;
        default:
          if (static_cast<unsigned char>(c) < 0x20) {
            char buf[8];
            std::snprintf(buf, sizeof(buf), "\\u%04x", c);
            out_ << buf;
          } else {
            out_ << c;
          }
      }
    }
    out_ << '"';
  }
};

namespace json_util {

// Returns the value (as raw substring without quotes) of the first "key": ...
// pair in `obj`. Used only for flat top-level requests, so this naive scan is
// safe enough. Returns empty string if not found.
inline std::string findRawValue(const std::string& obj, const std::string& key) {
  const std::string needle = "\"" + key + "\"";
  size_t p = obj.find(needle);
  if (p == std::string::npos) return {};
  p += needle.size();
  // skip whitespace and the colon
  while (p < obj.size() && (obj[p] == ' ' || obj[p] == '\t')) ++p;
  if (p >= obj.size() || obj[p] != ':') return {};
  ++p;
  while (p < obj.size() && (obj[p] == ' ' || obj[p] == '\t')) ++p;
  if (p >= obj.size()) return {};

  if (obj[p] == '"') {
    // quoted string
    ++p;
    std::string out;
    while (p < obj.size() && obj[p] != '"') {
      if (obj[p] == '\\' && p + 1 < obj.size()) {
        char next = obj[p + 1];
        switch (next) {
          case 'n':
            out += '\n';
            break;
          case 't':
            out += '\t';
            break;
          case 'r':
            out += '\r';
            break;
          default:
            out += next;
        }
        p += 2;
      } else {
        out += obj[p++];
      }
    }
    return out;
  } else {
    // raw token (number, true, false, null)
    std::string out;
    while (p < obj.size() && obj[p] != ',' && obj[p] != '}' && obj[p] != ' ' &&
           obj[p] != '\n' && obj[p] != '\t' && obj[p] != ']')
      out += obj[p++];
    return out;
  }
}

inline int getInt(const std::string& obj, const std::string& key,
                  int defaultVal = 0) {
  std::string s = findRawValue(obj, key);
  if (s.empty()) return defaultVal;
  try {
    return std::stoi(s);
  } catch (...) {
    return defaultVal;
  }
}

inline std::string getStr(const std::string& obj, const std::string& key) {
  return findRawValue(obj, key);
}

}  // namespace json_util
