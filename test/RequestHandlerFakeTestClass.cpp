#include "RequestHandlerFakeTestClass.h"

void RequestHandlerFakeTestClass::PrintToScreen(std::string ToPrint) {
  std::cout << ToPrint << std::endl;
}

std::string RequestHandlerFakeTestClass::GetAllTopics() {
  return this->KafkaConnection->GetAllTopics();
}
