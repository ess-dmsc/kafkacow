#pragma once

#include "ConnectKafkaInterface.h"
#include "TopicMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaFake : public ConnectKafkaInterface {
public:
  ConnectKafkaFake();

  std::string getAllTopics() override;

  KafkaMessageMetadataStruct consumeFromOffset() override;

  KafkaMessageMetadataStruct consumeLastNMessages() override;

  std::vector<OffsetsStruct>
  getTopicsHighAndLowOffsets(const std::string &Topic) override;

  OffsetsStruct getPartitionHighAndLowOffsets(const std::string &Topic,
                                              int32_t PartitionID) override;

  int getNumberOfTopicPartitions(std::string TopicName) override;

  void subscribeAtOffset(int64_t Offset, std::string TopicName) override;

  void subscribeToLastNMessages(int64_t NMessages, const std::string &TopicName,
                                int Partition) override;
  std::string showAllMetadata() override;
};
