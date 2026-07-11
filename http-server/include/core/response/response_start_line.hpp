#pragma once

#include <string>

#include "../statuses/http_statuses_list.hpp"

class ResponseStartLine {
public:
  ResponseStartLine(std::string protocol, HttpStatus::Code status_code);

  const std::string &get_protocol() const;
  const HttpStatus::Code get_status_code() const;
  const std::string &get_phrase() const;

  void set_status_code(HttpStatus::Code status_code);

  std::string to_string() const;

private:
  std::string _protocol;
  HttpStatus::Code _status_code;
  std::string _phrase;
};
