#pragma once
#include "ConnectKafkaInterface.h"
#include "TopicMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class ConnectKafkaFake : public ConnectKafkaInterface {
public:
  ConnectKafkaFake();
  std::unique_ptr<RdKafka::Metadata> queryMetadata() override;

  std::string getAllTopics() override;

  bool checkIfTopicExists(std::string Topic) override;

  std::pair<std::string, bool> consumeFromOffset() override;

  std::pair<std::string, bool>
  consumeLastNMessages() override;

  std::vector<int32_t>

  getTopicPartitionNumbers(std::string Topic) override;

  TopicMetadataStruct getTopicMetadata(std::string TopicName) override;

  std::unique_ptr<int64_t> getCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) override;

  std::vector<OffsetsStruct> getHighAndLowOffsets(std::string Topic) override;

  std::vector<RdKafka::TopicPartition *>
  getTopicPartitions(std::string Topic) override;

  int64_t getNumberOfTopicPartitions(std::string TopicName) override;

  void subscribeAtOffset(int64_t Offset, std::string TopicName) override;

  void subscribeToLastNMessages(int64_t NMessages,
                                std::string TopicName) override;
};
