#pragma once

#include "../OffsetsStruct.h"
#include "../TopicMetadataStruct.h"
#include "MessageMetadataStruct.h"
#include <CLI/CLI.hpp>
#include <librdkafka/rdkafkacpp.h>

namespace Kafka {

class ConsumerInterface {
public:
  virtual ~ConsumerInterface() = default;

  virtual std::string getAllTopics() = 0;

  virtual MessageMetadataStruct consume() = 0;

  virtual std::vector<OffsetsStruct>
  getTopicsHighAndLowOffsets(const std::string &Topic) = 0;

  virtual OffsetsStruct getPartitionHighAndLowOffsets(const std::string &Topic,
                                                      int32_t PartitionID) = 0;

  virtual int getNumberOfTopicPartitions(const std::string &TopicName) = 0;

  virtual void subscribeAtOffset(int64_t Offset,
                                 const std::string &TopicName) = 0;

  virtual void subscribeToLastNMessages(int64_t NMessages,
                                        const std::string &TopicName,
                                        int Partition) = 0;

  virtual std::string showAllMetadata() = 0;

  virtual int64_t getOffsetForDate(const std::string &Date,
                                   const std::string &Topic) = 0;
};
} // namespace Kafka
