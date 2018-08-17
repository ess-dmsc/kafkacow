#include "RequestHandler.h"
#include "ConnectKafka.h"

void RequestHandler::PrintToScreen(std::string ToPrint) {
  std::cout << this->KafkaConnection->GetAllTopics() << std::endl;
}
std::string RequestHandler::GetAllTopics() {
  return this->KafkaConnection->GetAllTopics();
}

bool RequestHandler::CheckIfTopicExists(std::string Topic) {
  return KafkaConnection->CheckIfTopicExists(Topic);
}

void RequestHandler::Consume(std::string Topic) {
  KafkaConnection->Consume(Topic);
}

void RequestHandler::SubscribeToTopic(const std::vector<std::string> &Topic) {
  KafkaConnection->SubscribeToTopic(Topic);
}
