#pragma once

#include <exception>
#include <string>

class ArgumentException : public std::exception {
public:
  explicit ArgumentException(std::string Message) : Message(Message) {}
  const char *what() const noexcept override { return Message.c_str(); }

private:
  std::string Message;
};

class TimeoutException : public std::exception {
public:
  explicit TimeoutException(std::string Message) : Message(Message) {}
  const char *what() const noexcept override { return Message.c_str(); }

private:
  std::string Message;
};
