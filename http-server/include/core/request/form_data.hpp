#pragma once

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "../headers/http_headers_list.hpp"

class FormData {
public:
  struct Binary {
    std::string filename;
    HttpHeaders::Values::ContentType::Type content_type;
    std::vector<char> data;
  };

  void push_item(std::string name, std::string value);
  void push_item(std::string name, Binary value);

  std::optional<std::variant<std::string, Binary>> get(const char *name) const;

private:
  std::map<std::string, std::variant<std::string, Binary>> _data;
};