#pragma once

#include <nlohmann/json.hpp>
#include <variant>

#include "../headers/headers.hpp"
#include "form_data.hpp"
#include "request_start_line.hpp"

using BodyType =
    std::variant<std::monostate, std::string, nlohmann::json, FormData>;

class Request {
public:
  using Params = std::map<std::string, std::string>;
  using QueryParams = std::map<std::string, std::string>;

  Request(RequestStartLine st, Headers headers, BodyType body,
          Params params = {}, QueryParams query_params = {});

  const RequestStartLine &get_start_line() const;
  const Headers &get_headers() const;
  std::optional<std::string>
  get_header(const HttpHeaders::Names::Type &name) const;
  const BodyType &get_body() const;

  const Params &get_params() const;
  std::optional<std::string> get_param(const std::string &name) const;

  const QueryParams &get_query_params() const;
  std::optional<std::string> get_query_param(const std::string &name) const;

private:
  const RequestStartLine _st;
  const Headers _headers;
  const BodyType _body;
  const Params _params;
  const QueryParams _query_params;
};
