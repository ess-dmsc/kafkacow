#pragma once

#include "ConnectKafkaInterface.h"
#include "OffsetsStruct.h"
#include "TopicMetadataStruct.h"
#include <librdkafka/rdkafkacpp.h>

class ConnectKafka : public ConnectKafkaInterface {
  std::shared_ptr<RdKafka::KafkaConsumer> Consumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;

public:
  ConnectKafka(std::string Broker, std::string ErrStr);
  ~ConnectKafka() {
    if (Consumer) {
      Consumer->close();
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

  std::unique_ptr<RdKafka::Metadata> queryMetadata() override;

  std::string getAllTopics() override;

  bool checkIfTopicExists(std::string Topic) override;

  std::pair<std::string, bool> consumeFromOffset() override;

  std::pair<std::string, bool> consumeLastNMessages() override;

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
