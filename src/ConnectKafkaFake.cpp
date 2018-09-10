#include "ConnectKafkaFake.h"

ConnectKafkaFake::ConnectKafkaFake() {}

std::string ConnectKafkaFake::getAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

std::pair<std::string, bool> ConnectKafkaFake::consumeFromOffset() {
  return std::pair<std::string, bool>("HiddenSecretMessageFromLovingNeutron",
                                      true);
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

int ConnectKafkaFake::getNumberOfTopicPartitions(std::string TopicName) {
  return 1;
}

void ConnectKafkaFake::subscribeAtOffset(int64_t Offset,
                                         std::string TopicName) {}

void ConnectKafkaFake::subscribeToLastNMessages(int64_t NMessages,
                                                const std::string &TopicName,
                                                int Partition) {}

std::pair<std::string, bool> ConnectKafkaFake::consumeLastNMessages() {
  return std::pair<std::string, bool>("HiddenSecretMessageFromLovingNeutron",
                                      true);
}
