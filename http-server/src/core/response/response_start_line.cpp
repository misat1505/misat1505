#include "../../../include/core/response/response_start_line.hpp"

#include <map>

ResponseStartLine::ResponseStartLine(std::string protocol,
                                     HttpStatus::Code status_code)
    : _protocol(protocol) {
  this->set_status_code(status_code);
}

const std::string &ResponseStartLine::get_protocol() const { return _protocol; }

const HttpStatus::Code ResponseStartLine::get_status_code() const {
  return _status_code;
}

const std::string &ResponseStartLine::get_phrase() const { return _phrase; }

void ResponseStartLine::set_status_code(HttpStatus::Code status_code) {
  _status_code = status_code;
  _phrase = HttpStatus::reason_phrase(status_code);
}

std::string ResponseStartLine::to_string() const {
  std::string result;
  result += _protocol + " " + std::to_string(HttpStatus::to_int(_status_code)) +
            " " + _phrase;

  return result;
}
