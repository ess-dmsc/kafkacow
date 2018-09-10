#pragma once

#include "ConnectKafkaInterface.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class ConnectKafka : public ConnectKafkaInterface {
private:
  std::shared_ptr<RdKafka::KafkaConsumer> Consumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;

  std::unique_ptr<RdKafka::Metadata> queryMetadata();

  TopicMetadataStruct getTopicMetadata(std::string TopicName);
  std::vector<int32_t> getTopicPartitionNumbers(std::string Topic);

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

  std::string getAllTopics() override;

  std::pair<std::string, bool> consumeFromOffset() override;

  std::pair<std::string, bool> consumeLastNMessages() override;

  std::vector<OffsetsStruct> getHighAndLowOffsets(std::string Topic) override;

  int getNumberOfTopicPartitions(std::string TopicName) override;

  void subscribeAtOffset(int64_t Offset, std::string TopicName) override;

  void subscribeToLastNMessages(int64_t NMessages, const std::string &TopicName,
                                int Partition) override;
};
