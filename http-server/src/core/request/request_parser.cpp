#include "../../../include/core/request/request_parser.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>

#include "../../../include/core/headers/http_headers_list.hpp"
#include "../../../include/core/request/form_data_parser.hpp"

RequestParser::RequestParser(const char *request, size_t request_size)
    : _raw_request(request), _raw_request_size(request_size) {
  _lines = {};

  std::istringstream stream(request);
  std::string line;

  while (std::getline(stream, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    _lines.push_back(line);
  }
}

Request RequestParser::parse() {
  RequestStartLine sl = this->parse_start_line();
  Headers headers = this->parse_headers();
  BodyType body = this->parse_body(headers);
  Request::QueryParams query_params =
      this->parse_query_params(sl.get_request_target());

  Request request = Request(sl, headers, body, {}, query_params);
  return request;
}

RequestStartLine RequestParser::parse_start_line() {
  std::istringstream lineStream(_lines[0]);
  std::string raw_method, target, protocol;
  lineStream >> raw_method >> target >> protocol;
  HttpMethods::Type method = HttpMethods::from_string(raw_method).value();
  RequestStartLine st = RequestStartLine(method, target, protocol);
  return st;
}

Headers RequestParser::parse_headers() {
  std::map<HttpHeaders::Names::Type, std::string> headers;

  for (size_t i = 1; i < _lines.size(); ++i) {
    const std::string &line = _lines[i];
    if (line.empty()) {
      break;
    }

    size_t pos = line.find(':');
    if (pos == std::string::npos)
      continue;

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    while (!value.empty() && value.front() == ' ')
      value.erase(0, 1);

    auto it = HttpHeaders::Names::from_string(key);
    if (it.has_value()) {
      headers[it.value()] = value;
    }
  }

  return headers;
}

BodyType RequestParser::parse_body(Headers &headers) {
  std::string content_type = headers.get()[HttpHeaders::Names::ContentType];

  if (content_type.rfind(
          HttpHeaders::Values::ContentType::to_string(
              HttpHeaders::Values::ContentType::MultipartFormData),
          0) == 0) {
    std::string boundary_key = "boundary=";
    size_t pos = content_type.find(boundary_key);
    if (pos != std::string::npos) {
      std::string boundary = content_type.substr(pos + boundary_key.size());
      return this->parse_form_data_body(boundary);
    }
  }

  auto it = std::find_if(_lines.begin(), _lines.end(),
                         [](const std::string line) { return line.empty(); });
  if (it == _lines.end()) {
    return std::monostate{};
  }

  auto bodyStart = std::next(it);
  if (bodyStart == _lines.end()) {
    return std::string{};
  }

  std::string body;
  for (auto lineIt = bodyStart; lineIt != _lines.end(); ++lineIt) {
    body += *lineIt + "\n";
  }

  if (headers.get()[HttpHeaders::Names::ContentType] ==
      HttpHeaders::Values::ContentType::to_string(
          HttpHeaders::Values::ContentType::ApplicationJson)) {
    return nlohmann::json::parse(body);
  }

  return body;
}

BodyType RequestParser::parse_form_data_body(const std::string &boundary) {
  auto it = std::find_if(_lines.begin(), _lines.end(),
                         [](const std::string line) { return line.empty(); });
  if (it == _lines.end()) {
    throw std::exception();
  }

  size_t body_offset = 0;
  for (auto line_it = _lines.begin(); line_it != std::next(it); ++line_it) {
    body_offset += line_it->size() + 2;
  }
  size_t body_size = _raw_request_size - body_offset;
  const char *body_ptr = _raw_request + body_offset;

  auto fdp = FormDataParser(body_ptr, body_size, boundary);
  return fdp.parse();
}

Request::QueryParams
RequestParser::parse_query_params(const std::string &request_target) {
  Request::QueryParams query_params;

  auto pos = request_target.find('?');
  if (pos == std::string::npos)
    return query_params;

  std::string query = request_target.substr(pos + 1);
  size_t start = 0;
  while (start < query.size()) {
    auto eq_pos = query.find('=', start);
    auto amp_pos = query.find('&', start);

    std::string key, value;

    if (eq_pos != std::string::npos) {
      key = query.substr(start, eq_pos - start);
      if (amp_pos != std::string::npos)
        value = query.substr(eq_pos + 1, amp_pos - eq_pos - 1);
      else
        value = query.substr(eq_pos + 1);
    } else {
      if (amp_pos != std::string::npos)
        key = query.substr(start, amp_pos - start);
      else
        key = query.substr(start);
    }

    if (!key.empty()) {
      query_params[key] = value;
    }

    if (amp_pos == std::string::npos)
      break;
    start = amp_pos + 1;
  }

  return query_params;
}