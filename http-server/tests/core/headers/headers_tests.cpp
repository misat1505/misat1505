#include "../../../include/core/headers/headers.hpp"
#include "../../../include/core/headers/http_headers_list.hpp"

#include <gtest/gtest.h>

TEST(HeadersTest, DefaultConstructorCreatesEmptyHeaders) {
  Headers headers;
  EXPECT_TRUE(headers.get().empty());
}

TEST(HeadersTest, ConstructorInitializesHeaders) {
  std::map<HttpHeaders::Names::Type, std::string> init = {
      {HttpHeaders::Names::ContentType,
       HttpHeaders::Values::ContentType::to_string(
           HttpHeaders::Values::ContentType::TextHtml)},
      {HttpHeaders::Names::ContentLength, "123"}};
  Headers headers(init);

  EXPECT_EQ(headers.get().size(), 2);
  EXPECT_EQ(headers.get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::TextHtml));
  EXPECT_EQ(headers.get()[HttpHeaders::Names::ContentLength], "123");
}

TEST(HeadersTest, SetAddsOrUpdatesHeader) {
  Headers headers;
  headers.set(HttpHeaders::Names::ContentType,
              HttpHeaders::Values::ContentType::to_string(
                  HttpHeaders::Values::ContentType::ApplicationJson));

  EXPECT_EQ(headers.get().size(), 1);
  EXPECT_EQ(headers.get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::ApplicationJson));

  headers.set(HttpHeaders::Names::ContentType,
              HttpHeaders::Values::ContentType::to_string(
                  HttpHeaders::Values::ContentType::TextPlain));
  EXPECT_EQ(headers.get().size(), 1);
  EXPECT_EQ(headers.get()[HttpHeaders::Names::ContentType],
            HttpHeaders::Values::ContentType::to_string(
                HttpHeaders::Values::ContentType::TextPlain));
}

TEST(HeadersTest, ToStringReturnsCorrectFormat) {
  Headers headers;
  headers.set(HttpHeaders::Names::ContentType,
              HttpHeaders::Values::ContentType::to_string(
                  HttpHeaders::Values::ContentType::TextHtml));
  headers.set(HttpHeaders::Names::ContentLength, "456");

  std::string str = headers.to_string();
  EXPECT_NE(str.find("Content-Type: text/html"), std::string::npos);
  EXPECT_NE(str.find("Content-Length: 456"), std::string::npos);
}
