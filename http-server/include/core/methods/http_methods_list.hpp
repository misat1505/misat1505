#pragma once

#include <map>
#include <optional>
#include <string>

namespace HttpMethods {
enum class Type { GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD };

inline const std::map<Type, std::string> TypeStrings = {
    {Type::GET, "GET"},     {Type::POST, "POST"},
    {Type::PUT, "PUT"},     {Type::DELETE, "DELETE"},
    {Type::PATCH, "PATCH"}, {Type::OPTIONS, "OPTIONS"},
    {Type::HEAD, "HEAD"}};

inline std::string to_string(Type method) {
  auto it = TypeStrings.find(method);
  return it != TypeStrings.end() ? it->second : "";
}

inline std::optional<Type> from_string(const std::string &s) {
  for (const auto &[method, str] : TypeStrings) {
    if (str == s)
      return method;
  }
  return std::nullopt;
}

} // namespace HttpMethods