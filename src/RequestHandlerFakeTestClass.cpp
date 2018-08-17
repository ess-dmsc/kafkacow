#include "RequestHandlerFakeTestClass.h"

void RequestHandlerFakeTestClass::PrintToScreen(std::string ToPrint) {
  std::cout << ToPrint << std::endl;
}

std::string RequestHandlerFakeTestClass::GetAllTopics() {
  return this->KafkaConnection->GetAllTopics();
}

bool RequestHandlerFakeTestClass::CheckIfTopicExists(std::string Topic) {
  return KafkaConnection->CheckIfTopicExists(Topic);
}

void RequestHandlerFakeTestClass::Consume(std::string Topic) {
  KafkaConnection->Consume(Topic);
}

void RequestHandlerFakeTestClass::SubscribeToTopic(
    const std::vector<std::string> &Topic) {
  KafkaConnection->SubscribeToTopic(Topic);
}