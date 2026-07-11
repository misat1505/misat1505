#include "../../../include/core/request/request_start_line.hpp"

RequestStartLine::RequestStartLine(HttpMethods::Type method,
                                   std::string request_target,
                                   std::string protocol)
    : _method(method), _request_target(request_target), _protocol(protocol) {}

HttpMethods::Type RequestStartLine::get_method() const { return _method; }

const std::string &RequestStartLine::get_request_target() const {
  return _request_target;
}

const std::string &RequestStartLine::get_protocol() const { return _protocol; }