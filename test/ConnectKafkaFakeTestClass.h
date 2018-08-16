#pragma once
#include "ConnectKafkaParentClass.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaFakeTestClass : public ConnectKafkaParentClass {
  std::shared_ptr<RdKafka::KafkaConsumer> Consumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;

public:
  std::shared_ptr<RdKafka::KafkaConsumer> GetConsumer() override;
  ConnectKafkaFakeTestClass(std::string Broker, std::string ErrStr);
  std::unique_ptr<RdKafka::Metadata> queryMetadata() override;
  std::string GetAllTopics() override;
};