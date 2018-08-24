#pragma once

#include <exception>
#include <string>
#include "iostream"
class ArgumentsException : public std::exception {
private:
  std::string Message;

public:
  ArgumentsException(std::string Message) : Message(Message) {}
  void printException() { std::cout << Message << std::endl; }
};
