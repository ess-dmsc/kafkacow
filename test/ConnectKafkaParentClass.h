#pragma once
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaParentClass {
public:
  virtual std::shared_ptr<RdKafka::KafkaConsumer> GetConsumer() = 0;
  virtual std::unique_ptr<RdKafka::Metadata> queryMetadata() = 0;
  virtual std::string GetAllTopics() = 0;
};