#pragma once

#include <exception>
#include <string>

class ArgumentsException : public std::exception {
public:
  explicit ArgumentsException(std::string Message) : Message(Message) {}
  const char *what() const noexcept override { return Message.c_str(); }

private:
  std::string Message;
};
