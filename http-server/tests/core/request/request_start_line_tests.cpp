#include "../../../include/core/request/request_start_line.hpp"

#include <gtest/gtest.h>

TEST(RequestStartLineTest, ConstructorAndGetters) {
  RequestStartLine rsl(HttpMethods::Type::GET, "/index.html", "HTTP/1.1");

  EXPECT_EQ(rsl.get_method(), HttpMethods::Type::GET);
  EXPECT_EQ(rsl.get_request_target(), "/index.html");
  EXPECT_EQ(rsl.get_protocol(), "HTTP/1.1");
}
