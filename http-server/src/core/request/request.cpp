#include "../../../include/core/request/request.hpp"

Request::Request(RequestStartLine st, Headers headers, BodyType body,
                 Params params, QueryParams query_params)
    : _st(std::move(st)), _headers(std::move(headers)), _body(std::move(body)),
      _params(std::move(params)), _query_params(std::move(query_params)) {}

const RequestStartLine &Request::get_start_line() const { return _st; }

const Headers &Request::get_headers() const { return _headers; }

std::optional<std::string>
Request::get_header(const HttpHeaders::Names::Type &name) const {
  auto it = _headers.get().find(name);
  if (it != _headers.get().end()) {
    return it->second;
  }
  return std::nullopt;
}

const BodyType &Request::get_body() const { return _body; }

const Request::Params &Request::get_params() const { return _params; }

std::optional<std::string> Request::get_param(const std::string &name) const {
  auto it = _params.find(name);
  if (it != _params.end()) {
    return it->second;
  }
  return std::nullopt;
}

const Request::QueryParams &Request::get_query_params() const {
  return _query_params;
}

std::optional<std::string>
Request::get_query_param(const std::string &name) const {
  auto it = _query_params.find(name);
  if (it != _query_params.end()) {
    return it->second;
  }
  return std::nullopt;
}
