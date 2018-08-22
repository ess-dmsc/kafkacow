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

  virtual std::pair<std::string, bool> ConsumeFromOffset(std::string Topic) = 0;
  virtual std::pair<std::string, bool>
  ConsumeLastNMessages(std::string Topic, int64_t NumberOfMessages) = 0;

  virtual std::vector<int32_t> GetTopicPartitionNumbers(std::string Topic) = 0;

  virtual TopicMetadataStruct GetTopicMetadata(std::string Topic) = 0;

  virtual std::unique_ptr<int64_t> GetCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) = 0;

  virtual std::vector<OffsetsStruct>

  GetHighAndLowOffsets(std::string Topic) = 0;

  virtual std::vector<RdKafka::TopicPartition *>
  GetTopicPartitions(std::string Topic) = 0;

  virtual int64_t GetNumberOfTopicPartitions(std::string TopicName) = 0;

  virtual void SubscribeAtOffset(int64_t Offset, std::string TopicName) = 0;

  virtual void SubscribeToLastNMessages(int64_t NMessages,
                                        std::string TopicName) = 0;
};
