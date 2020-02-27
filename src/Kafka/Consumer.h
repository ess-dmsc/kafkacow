#pragma once

#include "ConsumerInterface.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Kafka {

class Consumer : public ConsumerInterface {
public:
  explicit Consumer(std::string const &Broker);

  ~Consumer() override {
    if (KafkaConsumer) {
      KafkaConsumer->close();
      /*
       * Wait for RdKafka to decommission.
       * This is not strictly needed (with check outq_len() above), but
       * allows RdKafka to clean up all its resources before the application
       * exits so that memory profilers such as valgrind wont complain about
       * memory leaks.
       */
      RdKafka::wait_destroyed(5000);
    }
  }

  std::string getAllTopics() override;

  MessageMetadataStruct consume() override;

  std::vector<Metadata::Partition>
  getTopicsHighAndLowOffsets(const std::string &Topic) override;

  Metadata::Partition
  getPartitionHighAndLowOffsets(const std::string &Topic,
                                int32_t PartitionID) const override;

  int getNumberOfTopicPartitions(const std::string &Topic) override;

  void subscribeAtOffset(int64_t Offset, const std::string &Topic) override;

  void subscribeToLastNMessages(int64_t NMessages, const std::string &Topic,
                                int Partition) override;

  std::string showAllMetadata() override;

  std::unique_ptr<RdKafka::Metadata> queryMetadata() const;

  int64_t getOffsetForDate(const std::string &Date,
                           const std::string &Topic) override;

private:
  std::shared_ptr<RdKafka::KafkaConsumer> KafkaConsumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;
  std::shared_ptr<spdlog::logger> Logger;

  std::vector<int32_t> getTopicPartitionNumbers(const std::string &Topic);
};
}
