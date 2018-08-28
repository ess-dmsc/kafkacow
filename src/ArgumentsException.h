#pragma once

#include "iostream"
#include <exception>
#include <string>
class ArgumentsException : public std::exception {
private:
  std::string Message;

public:
  ArgumentsException(std::string Message) : Message(Message) {}
  void printException() { std::cout << Message << std::endl; }
};
