#include "../../../include/core/response/response.hpp"

#include <iostream>
#include <map>

#include "../../../include/core/headers/http_headers_list.hpp"

Response::Response(ResponseStartLine st) : _st(st) {
  std::map<HttpHeaders::Names::Type, std::string> h;
  _headers = Headers(h);
}

Headers &Response::get_headers() { return _headers; }

const Headers &Response::get_headers() const { return _headers; }

const HttpStatus::Code Response::get_status_code() const {
  return _st.get_status_code();
}

const std::string &Response::get_body() const { return _body; }

void Response::set_status_code(HttpStatus::Code status) {
  _st.set_status_code(status);
}

void Response::set_body(const char *text) { this->set_body(std::string(text)); }

void Response::set_body(std::string text) {
  _body = text;
  _headers.set(HttpHeaders::Names::ContentLength, std::to_string(text.size()));
  _headers.set(HttpHeaders::Names::Type::ContentType,
               HttpHeaders::Values::ContentType::to_string(
                   HttpHeaders::Values::ContentType::TextPlain));
}

void Response::set_body(nlohmann::json data) {
  _body = data.dump();
  _headers.set(HttpHeaders::Names::ContentLength, std::to_string(_body.size()));
  _headers.set(HttpHeaders::Names::ContentType,
               HttpHeaders::Values::ContentType::to_string(
                   HttpHeaders::Values::ContentType::ApplicationJson));
}

void Response::set_header(HttpHeaders::Names::Type name, std::string value) {
  _headers.set(name, value);
}

std::string Response::to_string() const {
  std::string result;
  result += _st.to_string() + "\n" + _headers.to_string() + "\n" + _body;

  return result;
}
