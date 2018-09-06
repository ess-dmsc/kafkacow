#include "ConnectKafkaFake.h"

ConnectKafkaFake::ConnectKafkaFake() {}

std::string ConnectKafkaFake::getAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

std::pair<std::string, bool> ConnectKafkaFake::consumeFromOffset() {
  return std::pair<std::string, bool>("HiddenSecretMessageFromLovingNeutron",
                                      true);
}

std::unique_ptr<int64_t> ConnectKafkaFake::getCurrentPartitionOffset(
    const RdKafka::TopicMetadata::PartitionMetadataVector *) {
  return std::unique_ptr<int64_t>();
}

std::vector<OffsetsStruct>
ConnectKafkaFake::getHighAndLowOffsets(std::string Topic) {
  std::vector<OffsetsStruct> VectorOfPartitions;
  OffsetsStruct FirstPartition = {1234, 12345, 0};
  OffsetsStruct SecondPartition{2234, 22345, 1};
  VectorOfPartitions.push_back(FirstPartition);
  VectorOfPartitions.push_back(SecondPartition);
  return VectorOfPartitions;
}

int64_t ConnectKafkaFake::getNumberOfTopicPartitions(std::string TopicName) {
  return 1;
}

void ConnectKafkaFake::subscribeAtOffset(int64_t Offset,
                                         std::string TopicName) {}

void ConnectKafkaFake::subscribeToLastNMessages(int64_t NMessages,
                                                std::string TopicName) {}

std::pair<std::string, bool> ConnectKafkaFake::consumeLastNMessages() {
  return std::pair<std::string, bool>("HiddenSecretMessageFromLovingNeutron",
                                      true);
}
