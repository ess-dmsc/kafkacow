#include "ConnectKafkaFake.h"

ConnectKafkaFake::ConnectKafkaFake() {}

std::string ConnectKafkaFake::getAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

KafkaMessageMetadataStruct ConnectKafkaFake::consumeFromOffset() {
  KafkaMessageMetadataStruct DataToReturn;
  DataToReturn.Payload = "HiddenSecretMessageFromLovingNeutron";
  DataToReturn.PartitionEOF = true;
  return DataToReturn;
}

std::vector<OffsetsStruct>
ConnectKafkaFake::getTopicsHighAndLowOffsets(std::string Topic) {
  std::vector<OffsetsStruct> VectorOfPartitions;
  OffsetsStruct FirstPartition = {1234, 12345, 0};
  OffsetsStruct SecondPartition{2234, 22345, 1};
  OffsetsStruct ThirdPartition = getPartitionHighAndLowOffsets(Topic, 3);
  VectorOfPartitions.push_back(FirstPartition);
  VectorOfPartitions.push_back(SecondPartition);
  VectorOfPartitions.push_back(ThirdPartition);
  return VectorOfPartitions;
}

int ConnectKafkaFake::getNumberOfTopicPartitions(std::string TopicName) {
  return 1;
}

void ConnectKafkaFake::subscribeAtOffset(int32_t Offset,
                                         std::string TopicName) {}

void ConnectKafkaFake::subscribeToLastNMessages(int32_t NMessages,
                                                const std::string &TopicName,
                                                int Partition) {}

KafkaMessageMetadataStruct ConnectKafkaFake::consumeLastNMessages() {
  return consumeFromOffset();
}

std::string ConnectKafkaFake::showAllMetadata() { return "Test return"; }

OffsetsStruct
ConnectKafkaFake::getPartitionHighAndLowOffsets(const std::string &Topic,
                                                int32_t PartitionID) {
  OffsetsStruct OffsetsToReturn;
  OffsetsToReturn.HighOffset = 5;
  OffsetsToReturn.LowOffset = 1;
  OffsetsToReturn.PartitionId = PartitionID;
  return OffsetsToReturn;
}
