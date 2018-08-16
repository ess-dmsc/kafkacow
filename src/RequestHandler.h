#pragma once

#include "ConnectKafka.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class RequestHandler {
private:
  std::shared_ptr<RdKafka::KafkaConsumer> Consumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;
  std::unique_ptr<ConnectKafka> KafkaConnection;

public:
  explicit RequestHandler(std::unique_ptr<ConnectKafka> KafkaConnection)
      : KafkaConnection(std::move(KafkaConnection)) {
    Consumer = this->KafkaConnection->GetConsumer();
    MetadataPointer = this->KafkaConnection->queryMetadata();
  }
  void PrintToScreen(std::string ToPrint);
  std::string GetAllTopics();
};
