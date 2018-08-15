//
// Created by michal on 15/08/18.
//

#include "RequestHandler.h"
#include "ConnectKafka.h"

std::string RequestHandler::PrintAllTopics() {
  auto MetadataPointer = queryMetadata(Consumer);
  auto Topics = MetadataPointer->topics();
  for (const auto &TopicName : *Topics) {
    std::cout << TopicName->topic() << std::endl;
  }
}
