#include "RequestHandler.h"
#include "ConnectKafka.h"

void RequestHandler::PrintToScreen(std::string ToPrint) {
  std::cout << this->KafkaConnection->GetAllTopics() << std::endl;
}
std::string RequestHandler::GetAllTopics() {
  return this->KafkaConnection->GetAllTopics();
}
