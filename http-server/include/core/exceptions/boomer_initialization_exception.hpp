#pragma once

#include <stdexcept>
#include <string>

class BoomerInitializationException : public std::exception {
public:
  explicit BoomerInitializationException(const std::string &message);
  const char *what() const noexcept override;

private:
  std::string _message;
};
