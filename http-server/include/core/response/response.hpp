#pragma once

#include <nlohmann/json.hpp>

#include "../headers/headers.hpp"
#include "../headers/http_headers_list.hpp"
#include "../statuses/http_statuses_list.hpp"
#include "response_start_line.hpp"

class Response {
public:
  Response(ResponseStartLine st);

  Headers &get_headers();
  const Headers &get_headers() const;
  const std::string &get_body() const;
  const HttpStatus::Code get_status_code() const;

  void set_status_code(HttpStatus::Code status);

  void set_body(const char *text);
  void set_body(std::string text);
  void set_body(nlohmann::json data);

  void set_header(HttpHeaders::Names::Type name, std::string value);

  std::string to_string() const;

private:
  ResponseStartLine _st;
  Headers _headers;
  std::string _body;
};
