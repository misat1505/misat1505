#pragma once

#include <map>
#include <string>

namespace HttpStatus {

enum class Code {
  // 1xx Informational
  CONTINUE,
  SWITCHING_PROTOCOLS,
  PROCESSING,

  // 2xx Success
  OK,
  CREATED,
  ACCEPTED,
  NON_AUTHORITATIVE_INFORMATION,
  NO_CONTENT,
  RESET_CONTENT,
  PARTIAL_CONTENT,

  // 3xx Redirection
  MULTIPLE_CHOICES,
  MOVED_PERMANENTLY,
  FOUND,
  SEE_OTHER,
  NOT_MODIFIED,
  TEMPORARY_REDIRECT,
  PERMANENT_REDIRECT,

  // 4xx Client Error
  BAD_REQUEST,
  UNAUTHORIZED,
  FORBIDDEN,
  NOT_FOUND,
  METHOD_NOT_ALLOWED,
  NOT_ACCEPTABLE,
  REQUEST_TIMEOUT,
  CONFLICT,
  GONE,
  LENGTH_REQUIRED,
  PAYLOAD_TOO_LARGE,
  URI_TOO_LONG,
  UNSUPPORTED_MEDIA_TYPE,
  TOO_MANY_REQUESTS,

  // 5xx Server Error
  INTERNAL_SERVER_ERROR,
  NOT_IMPLEMENTED,
  BAD_GATEWAY,
  SERVICE_UNAVAILABLE,
  GATEWAY_TIMEOUT
};

inline const std::map<Code, int> CodeToInt = {
    {Code::CONTINUE, 100},
    {Code::SWITCHING_PROTOCOLS, 101},
    {Code::PROCESSING, 102},
    {Code::OK, 200},
    {Code::CREATED, 201},
    {Code::ACCEPTED, 202},
    {Code::NON_AUTHORITATIVE_INFORMATION, 203},
    {Code::NO_CONTENT, 204},
    {Code::RESET_CONTENT, 205},
    {Code::PARTIAL_CONTENT, 206},
    {Code::MULTIPLE_CHOICES, 300},
    {Code::MOVED_PERMANENTLY, 301},
    {Code::FOUND, 302},
    {Code::SEE_OTHER, 303},
    {Code::NOT_MODIFIED, 304},
    {Code::TEMPORARY_REDIRECT, 307},
    {Code::PERMANENT_REDIRECT, 308},
    {Code::BAD_REQUEST, 400},
    {Code::UNAUTHORIZED, 401},
    {Code::FORBIDDEN, 403},
    {Code::NOT_FOUND, 404},
    {Code::METHOD_NOT_ALLOWED, 405},
    {Code::NOT_ACCEPTABLE, 406},
    {Code::REQUEST_TIMEOUT, 408},
    {Code::CONFLICT, 409},
    {Code::GONE, 410},
    {Code::LENGTH_REQUIRED, 411},
    {Code::PAYLOAD_TOO_LARGE, 413},
    {Code::URI_TOO_LONG, 414},
    {Code::UNSUPPORTED_MEDIA_TYPE, 415},
    {Code::TOO_MANY_REQUESTS, 429},
    {Code::INTERNAL_SERVER_ERROR, 500},
    {Code::NOT_IMPLEMENTED, 501},
    {Code::BAD_GATEWAY, 502},
    {Code::SERVICE_UNAVAILABLE, 503},
    {Code::GATEWAY_TIMEOUT, 504}};

inline const std::map<Code, std::string> CodeToReason = {
    {Code::CONTINUE, "Continue"},
    {Code::SWITCHING_PROTOCOLS, "Switching Protocols"},
    {Code::PROCESSING, "Processing"},
    {Code::OK, "OK"},
    {Code::CREATED, "Created"},
    {Code::ACCEPTED, "Accepted"},
    {Code::NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information"},
    {Code::NO_CONTENT, "No Content"},
    {Code::RESET_CONTENT, "Reset Content"},
    {Code::PARTIAL_CONTENT, "Partial Content"},
    {Code::MULTIPLE_CHOICES, "Multiple Choices"},
    {Code::MOVED_PERMANENTLY, "Moved Permanently"},
    {Code::FOUND, "Found"},
    {Code::SEE_OTHER, "See Other"},
    {Code::NOT_MODIFIED, "Not Modified"},
    {Code::TEMPORARY_REDIRECT, "Temporary Redirect"},
    {Code::PERMANENT_REDIRECT, "Permanent Redirect"},
    {Code::BAD_REQUEST, "Bad Request"},
    {Code::UNAUTHORIZED, "Unauthorized"},
    {Code::FORBIDDEN, "Forbidden"},
    {Code::NOT_FOUND, "Not Found"},
    {Code::METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {Code::NOT_ACCEPTABLE, "Not Acceptable"},
    {Code::REQUEST_TIMEOUT, "Request Timeout"},
    {Code::CONFLICT, "Conflict"},
    {Code::GONE, "Gone"},
    {Code::LENGTH_REQUIRED, "Length Required"},
    {Code::PAYLOAD_TOO_LARGE, "Payload Too Large"},
    {Code::URI_TOO_LONG, "URI Too Long"},
    {Code::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"},
    {Code::TOO_MANY_REQUESTS, "Too Many Requests"},
    {Code::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {Code::NOT_IMPLEMENTED, "Not Implemented"},
    {Code::BAD_GATEWAY, "Bad Gateway"},
    {Code::SERVICE_UNAVAILABLE, "Service Unavailable"},
    {Code::GATEWAY_TIMEOUT, "Gateway Timeout"}};

inline int to_int(Code code) {
  auto it = CodeToInt.find(code);
  return it != CodeToInt.end() ? it->second : 0;
}

inline std::string reason_phrase(Code code) {
  auto it = CodeToReason.find(code);
  return it != CodeToReason.end() ? it->second : "";
}

} // namespace HttpStatus
