#pragma once

#include "../TopicMetadataStruct.h"
#include "ConsumerInterface.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

namespace Kafka {

class FakeConsumer : public ConsumerInterface {
public:
  FakeConsumer();

  std::string getAllTopics() override;

  MessageMetadataStruct consume() override;

  std::vector<OffsetsStruct>
  getTopicsHighAndLowOffsets(const std::string &Topic) override;

  OffsetsStruct getPartitionHighAndLowOffsets(const std::string &Topic,
                                              int32_t PartitionID) override;

  int getNumberOfTopicPartitions(std::string TopicName) override;

  void subscribeAtOffset(int64_t Offset, std::string TopicName) override;

  void subscribeToLastNMessages(int64_t NMessages, const std::string &TopicName,
                                int Partition) override;

  std::string showAllMetadata() override;

  void subscribeToDate(const std::string &Topic,
                       const std::string &isoDate) override;
};
}
