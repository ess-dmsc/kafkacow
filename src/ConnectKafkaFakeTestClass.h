#pragma once
#include "ConnectKafkaParentClass.h"
#include "TopicMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaFakeTestClass : public ConnectKafkaParentClass {
public:
  ConnectKafkaFakeTestClass();
  std::unique_ptr<RdKafka::Metadata> queryMetadata() override;
  std::string GetAllTopics() override;
  void SubscribeToTopic(const std::vector<std::string> &Topic) override;
  bool CheckIfTopicExists(std::string Topic) override;
  virtual std::string Consume(std::string Topic) override;
  virtual std::vector<int32_t> GetTopicPartitions(std::string Topic) override;
  virtual std::unique_ptr<int64_t> GetCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) override;
  TopicMetadataStruct GetTopicMetadata(std::string Topic) override;
  virtual std::vector<OffsetsStruct>
  GetHighAndLowOffsets(std::string Topic) override;
};