#pragma once

#include <string>

#include "../methods/http_methods_list.hpp"

class RequestStartLine {
public:
  RequestStartLine(HttpMethods::Type method, std::string request_target,
                   std::string protocol);

  HttpMethods::Type get_method() const;
  const std::string &get_request_target() const;
  const std::string &get_protocol() const;

private:
  HttpMethods::Type _method;
  std::string _request_target;
  std::string _protocol;
};