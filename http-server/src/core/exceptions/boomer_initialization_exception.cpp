#include "../../../include/core/exceptions/boomer_initialization_exception.hpp"

BoomerInitializationException::BoomerInitializationException(
    const std::string &message)
    : _message(message) {}

const char *BoomerInitializationException::what() const noexcept {
  return _message.c_str();
}
