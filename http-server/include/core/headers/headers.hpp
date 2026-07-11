#pragma once

#include <map>
#include <string>

#include "http_headers_list.hpp"

class Headers {
  using HeadersMap = std::map<HttpHeaders::Names::Type, std::string>;

public:
  Headers() = default;
  Headers(HeadersMap headers);

  HeadersMap &get();
  const HeadersMap &get() const;
  void set(HttpHeaders::Names::Type name, std::string value);

  std::string to_string() const;

private:
  HeadersMap _headers;
};
