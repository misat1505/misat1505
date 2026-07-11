#include "../../../include/core/exceptions/boomer_initialization_exception.hpp"
#include <gtest/gtest.h>

TEST(BoomerInitializationExceptionTest, StoresMessageCorrectly) {
  std::string msg = "Initialization failed";
  BoomerInitializationException ex(msg);

  EXPECT_STREQ(ex.what(), msg.c_str());
}

TEST(BoomerInitializationExceptionTest, ReturnsConsistentMessage) {
  std::string msg = "Boomer system error";
  BoomerInitializationException ex(msg);

  const char *firstCall = ex.what();
  const char *secondCall = ex.what();

  EXPECT_STREQ(firstCall, msg.c_str());
  EXPECT_EQ(firstCall, secondCall);
}

TEST(BoomerInitializationExceptionTest, CanBeThrownAndCaught) {
  std::string msg = "Boomer init exception throw test";

  try {
    throw BoomerInitializationException(msg);
    FAIL() << "Exception was not thrown";
  } catch (const BoomerInitializationException &ex) {
    EXPECT_STREQ(ex.what(), msg.c_str());
  } catch (...) {
    FAIL() << "Unexpected exception type caught";
  }
}
