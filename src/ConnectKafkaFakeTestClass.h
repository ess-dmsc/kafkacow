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

  virtual std::pair<std::string, bool>
  ConsumeFromOffset(std::string Topic) override;

  virtual std::pair<std::string, bool>
  ConsumeLastNMessages(std::string Topic, int64_t NumberOfMessages) override;

  virtual std::vector<int32_t>

  GetTopicPartitionNumbers(std::string Topic) override;

  TopicMetadataStruct GetTopicMetadata(std::string TopicName) override;

  virtual std::unique_ptr<int64_t> GetCurrentPartitionOffset(
      const RdKafka::TopicMetadata::PartitionMetadataVector *) override;

  virtual std::vector<OffsetsStruct>
  GetHighAndLowOffsets(std::string Topic) override;

  virtual std::vector<RdKafka::TopicPartition *>
  GetTopicPartitions(std::string Topic) override;

  virtual int64_t GetNumberOfTopicPartitions(std::string TopicName) override;

  virtual void SubscribeAtOffset(int64_t Offset,
                                 std::string TopicName) override;

  virtual void SubscribeToLastNMessages(int64_t NMessages,
                                        std::string TopicName) override;
};