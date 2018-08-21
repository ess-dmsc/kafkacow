#pragma once
#include "OffsetsStruct.h"
#include "TopicMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaParentClass {
public:
  virtual std::unique_ptr<RdKafka::Metadata> queryMetadata() = 0;
  virtual std::string GetAllTopics() = 0;
  virtual void SubscribeToTopic(const std::vector<std::string> &Topic) = 0;
  virtual bool CheckIfTopicExists(std::string Topic) = 0;
  virtual std::string Consume(std::string Topic) = 0;
  virtual std::vector<int32_t> GetTopicPartitions(std::string Topic) = 0;
  virtual TopicMetadataStruct GetTopicMetadata(std::string Topic) = 0;
  virtual std::unique_ptr<int64_t> GetCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) = 0;
  virtual std::vector<OffsetsStruct>
  GetHighAndLowOffsets(std::string Topic) = 0;
};
