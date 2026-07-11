#include "../../../include/core/headers/headers.hpp"
#include "../../../include/core/headers/http_headers_list.hpp"
#include "../../../include/core/request/request.hpp"
#include "../../../include/core/request/request_start_line.hpp"

#include <gtest/gtest.h>

TEST(RequestTest, ConstructorInitializesStartLineAndHeaders) {
  RequestStartLine st(HttpMethods::Type::GET, "/index.html", "HTTP/1.1");
  Headers headers;
  headers.set(HttpHeaders::Names::ContentType,
              HttpHeaders::Values::ContentType::to_string(
                  HttpHeaders::Values::ContentType::TextHtml));

  Request request(st, headers, std::string{});

  EXPECT_EQ(request.get_start_line().get_method(), HttpMethods::Type::GET);
  EXPECT_EQ(request.get_start_line().get_request_target(), "/index.html");
  EXPECT_EQ(request.get_start_line().get_protocol(), "HTTP/1.1");

  EXPECT_EQ(request.get_headers().get().size(), 1);

  auto it = request.get_headers().get().find(HttpHeaders::Names::ContentType);
  EXPECT_NE(it, request.get_headers().get().end());
  EXPECT_EQ(it->second, HttpHeaders::Values::ContentType::to_string(
                            HttpHeaders::Values::ContentType::TextHtml));
}

TEST(RequestTest, GetStartLineReturnsReference) {
  RequestStartLine st(HttpMethods::Type::POST, "/submit", "HTTP/1.1");
  Headers headers;
  Request request(st, headers, std::string{});

  auto &ref = request.get_start_line();

  EXPECT_EQ(request.get_start_line().get_request_target(), "/submit");
}
