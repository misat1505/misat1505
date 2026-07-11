#include "../../../include/core/response/response_start_line.hpp"

#include <gtest/gtest.h>

TEST(ResponseStartLineTest, ConstructorSetsValues) {
  ResponseStartLine line("HTTP/1.1", HttpStatus::Code::OK);

  EXPECT_EQ(line.get_protocol(), "HTTP/1.1");
  EXPECT_EQ(line.get_status_code(), HttpStatus::Code::OK);
  EXPECT_EQ(line.get_phrase(), "OK");
}

TEST(ResponseStartLineTest, SetStatusCodeUpdatesPhrase) {
  ResponseStartLine line("HTTP/1.1", HttpStatus::Code::OK);
  line.set_status_code(HttpStatus::Code::OK);

  EXPECT_EQ(line.get_status_code(), HttpStatus::Code::OK);
  EXPECT_EQ(line.get_phrase(), "OK");
}

TEST(ResponseStartLineTest, ToStringReturnsCorrectFormat) {
  ResponseStartLine line("HTTP/1.1", HttpStatus::Code::OK);
  std::string expected = "HTTP/1.1 200 OK";

  EXPECT_EQ(line.to_string(), expected);
}
