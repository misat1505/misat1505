#include "../../../include/core/request/request_parser.hpp"
#include <gtest/gtest.h>

#include <iostream>

// Stub FormDataParser for testing parse_form_data_body
class FormDataParser {
public:
  FormDataParser(const char *body, size_t size, const std::string &boundary) {
    (void)body;
    (void)size;
    (void)boundary;
  }
  BodyType parse() {
    return std::map<std::string, std::string>{{"field", "value"}};
  }
};

TEST(RequestParserTest, ParsesStartLineCorrectly) {
  const char *raw = "GET /test?foo=bar HTTP/1.1\r\n"
                    "\r\n";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();

  EXPECT_EQ(request.get_start_line().get_method(), HttpMethods::Type::GET);
  EXPECT_EQ(request.get_start_line().get_request_target(), "/test?foo=bar");
  EXPECT_EQ(request.get_start_line().get_protocol(), "HTTP/1.1");
}

TEST(RequestParserTest, ParsesHeadersCorrectly) {
  const char *raw = "POST /submit HTTP/1.1\r\n"
                    "Host: example.com\r\n"
                    "Content-Type: application/json\r\n"
                    "\r\n";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &headers = request.get_headers().get();

  EXPECT_EQ(headers.at(HttpHeaders::Names::Host), "example.com");
  EXPECT_EQ(headers.at(HttpHeaders::Names::ContentType), "application/json");
}

TEST(RequestParserTest, ParsesBodyAsString) {
  const char *raw = "POST /submit HTTP/1.1\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    "Hello World";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &body = request.get_body();

  ASSERT_TRUE(std::holds_alternative<std::string>(body));
  EXPECT_EQ(std::get<std::string>(body), "Hello World\n");
}

TEST(RequestParserTest, ParsesBodyAsJson) {
  const char *raw = "POST /data HTTP/1.1\r\n"
                    "Content-Type: application/json\r\n"
                    "\r\n"
                    "{ \"key\": \"value\" }";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &body = request.get_body();

  ASSERT_TRUE(std::holds_alternative<nlohmann::json>(body));
  auto json = std::get<nlohmann::json>(body);
  EXPECT_EQ(json["key"], "value");
}

TEST(RequestParserTest, ParsesQueryParams) {
  const char *raw = "GET /search?q=unit+test&lang=en HTTP/1.1\r\n"
                    "\r\n";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &params = request.get_query_params();

  EXPECT_EQ(params.at("q"), "unit+test");
  EXPECT_EQ(params.at("lang"), "en");
}

TEST(RequestParserTest, ReturnsEmptyBodyIfNoContent) {
  const char *raw = "GET /empty HTTP/1.1\r\n"
                    "\r\n";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &body = request.get_body();

  EXPECT_TRUE(std::holds_alternative<std::string>(body));
}

TEST(RequestParserTest, ParsesMultipartFormDataBody) {
  const char *raw = "POST /upload HTTP/1.1\r\n"
                    "Content-Type: multipart/form-data; boundary=----abc\r\n"
                    "\r\n"
                    "------abc\r\n"
                    "Content-Disposition: form-data; name=\"field\"\r\n\r\n"
                    "value\r\n"
                    "------abc--\r\n";
  RequestParser parser(raw, strlen(raw));

  Request request = parser.parse();
  auto &body = request.get_body();

  ASSERT_TRUE(std::holds_alternative<FormData>(body));
  auto formData = std::get<FormData>(body);
  EXPECT_EQ(std::get<std::string>(*formData.get("field")), "value");
}
