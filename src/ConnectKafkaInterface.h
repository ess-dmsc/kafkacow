#pragma once

#include "OffsetsStruct.h"
#include "TopicMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaInterface {
public:
  virtual std::unique_ptr<RdKafka::Metadata> queryMetadata() = 0;

  virtual std::string getAllTopics() = 0;

  virtual bool checkIfTopicExists(std::string Topic) = 0;

  virtual std::pair<std::string, bool> consumeFromOffset() = 0;
  virtual std::pair<std::string, bool> consumeLastNMessages() = 0;

  virtual std::vector<int32_t> getTopicPartitionNumbers(std::string Topic) = 0;

  virtual TopicMetadataStruct getTopicMetadata(std::string Topic) = 0;

  virtual std::unique_ptr<int64_t> getCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) = 0;

  virtual std::vector<OffsetsStruct>

  getHighAndLowOffsets(std::string Topic) = 0;

  virtual std::vector<RdKafka::TopicPartition *>
  getTopicPartitions(std::string Topic) = 0;

  virtual int64_t getNumberOfTopicPartitions(std::string TopicName) = 0;

  virtual void subscribeAtOffset(int64_t Offset, std::string TopicName) = 0;

  virtual void subscribeToLastNMessages(int64_t NMessages,
                                        std::string TopicName) = 0;
};
