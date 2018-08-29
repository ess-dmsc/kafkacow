#pragma once

#include "iostream"
#include <exception>
#include <string>

class ArgumentsException : public std::exception {
private:
  std::string Message;

public:
  explicit ArgumentsException(std::string Message) : Message(Message) {}
  const char *what() const noexcept override { return Message.c_str(); }
};
