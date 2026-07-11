#pragma once

#include <string>
#include <vector>

#include "../headers/headers.hpp"
#include "request.hpp"
#include "request_start_line.hpp"

class RequestParser {
public:
  RequestParser(const char *request, size_t request_size);
  Request parse();

private:
  const char *_raw_request;
  size_t _raw_request_size;
  std::vector<std::string> _lines;

  RequestStartLine parse_start_line();
  Headers parse_headers();
  BodyType parse_body(Headers &headers);
  Request::QueryParams parse_query_params(const std::string &request_target);
  BodyType parse_form_data_body(const std::string &boundary);
};