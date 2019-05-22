#pragma once

#include "../OffsetsStruct.h"
#include "../TopicMetadataStruct.h"
#include "KafkaMessageMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

class ConsumerInterface {
public:
  virtual ~ConsumerInterface() = default;

  virtual std::string getAllTopics() = 0;

  virtual KafkaMessageMetadataStruct consume() = 0;

  virtual std::vector<OffsetsStruct>
  getTopicsHighAndLowOffsets(const std::string &Topic) = 0;

  virtual OffsetsStruct getPartitionHighAndLowOffsets(const std::string &Topic,
                                                      int32_t PartitionID) = 0;

  virtual int getNumberOfTopicPartitions(std::string TopicName) = 0;

  virtual void subscribeAtOffset(int64_t Offset, std::string TopicName) = 0;

  virtual void subscribeToLastNMessages(int64_t NMessages,
                                        const std::string &TopicName,
                                        int Partition) = 0;

  virtual std::string showAllMetadata() = 0;
};