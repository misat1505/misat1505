#include "../../../include/core/headers/headers.hpp"
#include "../../../include/core/headers/http_headers_list.hpp"
#include "../../../include/core/response/response.hpp"
#include "../../../include/core/response/response_start_line.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TEST(ResponseTest, ConstructorInitializesStartLineAndHeaders) {
  ResponseStartLine st("HTTP/1.1", HttpStatus::Code::OK);
  Response resp(st);

  EXPECT_EQ(resp.get_status_code(), HttpStatus::Code::OK);

  EXPECT_TRUE(resp.get_headers().get().empty());

  EXPECT_EQ(resp.get_body(), "");
}

TEST(ResponseTest, SetBodyStringUpdatesBodyAndHeaders) {
  ResponseStartLine st("HTTP/1.1", HttpStatus::Code::OK);
  Response resp(st);

  std::string bodyText = "Hello World";
  resp.set_body(bodyText);

  EXPECT_EQ(resp.get_body(), bodyText);
  EXPECT_EQ(resp.get_headers().get()[HttpHeaders::Names::ContentLength],
            std::to_string(bodyText.size()));
  EXPECT_EQ(resp.get_headers().get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::TextPlain));
}

TEST(ResponseTest, SetBodyJsonUpdatesBodyAndHeaders) {
  ResponseStartLine st("HTTP/1.1", HttpStatus::Code::OK);
  Response resp(st);

  json j = {{"key", "value"}};
  resp.set_body(j);

  EXPECT_EQ(resp.get_body(), j.dump());
  EXPECT_EQ(resp.get_headers().get()[HttpHeaders::Names::ContentLength],
            std::to_string(j.dump().size()));
  EXPECT_EQ(resp.get_headers().get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::ApplicationJson));
}

TEST(ResponseTest, SetHeaderOverridesHeader) {
  ResponseStartLine st("HTTP/1.1", HttpStatus::Code::OK);
  Response resp(st);

  resp.set_header(HttpHeaders::Names::ContentType,
                  HttpHeaders::Values::ContentType::to_string(
                      HttpHeaders::Values::ContentType::ImagePng));

  EXPECT_EQ(resp.get_headers().get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::ImagePng));
}

TEST(ResponseTest, ToStringIncludesStartLineHeadersAndBody) {
  ResponseStartLine st("HTTP/1.1", HttpStatus::Code::OK);
  Response resp(st);

  std::string bodyText = "Hello";
  resp.set_body(bodyText);

  std::string str = resp.to_string();
  EXPECT_NE(str.find("HTTP/1.1 200 OK"), std::string::npos);
  EXPECT_NE(str.find("Content-Length: 5"), std::string::npos);
  EXPECT_NE(str.find("Content-Type: text/plain"), std::string::npos);
  EXPECT_NE(str.find(bodyText), std::string::npos);
}
