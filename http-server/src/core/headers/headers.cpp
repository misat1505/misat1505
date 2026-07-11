#include "../../../include/core/headers/headers.hpp"

Headers::Headers(HeadersMap headers) : _headers(headers) {}

Headers::HeadersMap &Headers::get() { return _headers; }

const Headers::HeadersMap &Headers::get() const { return _headers; }

void Headers::set(HttpHeaders::Names::Type name, std::string value) {
  _headers[name] = value;
}

std::string Headers::to_string() const {
  std::string result;
  for (auto &[key, value] : _headers) {
    result += HttpHeaders::Names::to_string(key) + ": " + value + "\n";
  }

  return result;
}