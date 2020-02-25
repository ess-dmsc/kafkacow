#pragma once

#include "../TopicMetadataStruct.h"
#include "ConsumerInterface.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

namespace Kafka {

class FakeConsumer : public ConsumerInterface {
public:
  FakeConsumer() = default;

  std::string getAllTopics() override;

  MessageMetadataStruct consume() override;

  std::vector<Metadata::Partition>
  getTopicsHighAndLowOffsets(const std::string &Topic) override;

  Metadata::Partition
  getPartitionHighAndLowOffsets(const std::string &Topic,
                                int32_t PartitionID) override;

  int getNumberOfTopicPartitions(const std::string &TopicName) override;

  void subscribeAtOffset(int64_t Offset, const std::string &TopicName) override;

  void subscribeToLastNMessages(int64_t NMessages, const std::string &TopicName,
                                int Partition) override;

  std::string showAllMetadata() override;

  int64_t getOffsetForDate(const std::string &Date,
                           const std::string &Topic) override;
};
}
