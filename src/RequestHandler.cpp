#include "RequestHandler.h"
#include "ConnectKafka.h"

std::string RequestHandler::GetAllTopics() {
  // auto MetadataPointer = ConnectKafka::queryMetadata(Consumer);
  auto Topics = MetadataPointer->topics();
  std::string ListOfTopics = "";
  for (const auto &TopicName : *Topics) {
    ListOfTopics.append(TopicName->topic());
  }
  PrintToScreen(ListOfTopics);
}

void RequestHandler::PrintToScreen(std::string ToPrint) {
  std::cout << ToPrint << std::endl;
}
