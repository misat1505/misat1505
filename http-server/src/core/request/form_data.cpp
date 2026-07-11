#include "../../../include/core/request/form_data.hpp"

#include <iostream>

void FormData::push_item(std::string name, std::string value) {
  _data[name] = value;
}

void FormData::push_item(std::string name, Binary value) {

  _data[name] = value;
}

std::optional<std::variant<std::string, FormData::Binary>>
FormData::get(const char *name) const {
  auto it = _data.find(name);
  if (it != _data.end()) {
    return it->second;
  }
  return std::nullopt;
}
