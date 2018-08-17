#pragma once
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaParentClass {
public:
  virtual std::unique_ptr<RdKafka::Metadata> queryMetadata() = 0;
  virtual std::string GetAllTopics() = 0;
  virtual std::string
  SubscribeToTopic(const std::vector<std::string> &Topic) = 0;
  virtual bool CheckIfTopicExists(std::string Topic) = 0;
};