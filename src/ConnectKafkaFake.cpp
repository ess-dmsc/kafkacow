#include "ConnectKafkaFake.h"

ConnectKafkaFake::ConnectKafkaFake() {}

ConnectKafkaFake::ConnectKafkaFake(bool ReturnKey) : ReturnKey(ReturnKey) {}

std::string ConnectKafkaFake::getAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

KafkaMessageMetadataStruct ConnectKafkaFake::consume() {
  KafkaMessageMetadataStruct DataToReturn;
  DataToReturn.Payload = "HiddenSecretMessageFromLovingNeutron";
  DataToReturn.PartitionEOF = true;
  DataToReturn.Timestamp = 1542;
  DataToReturn.Offset = 1234;
  DataToReturn.Partition = 0;
  if (ReturnKey) {
    DataToReturn.KeyPresent = true;
    DataToReturn.Key = "MessageKey";
  }
  return DataToReturn;
}

std::vector<OffsetsStruct>
ConnectKafkaFake::getTopicsHighAndLowOffsets(const std::string &Topic) {

  std::vector<OffsetsStruct> VectorOfPartitions;

  if (Topic != "EmptyTopic") {
    OffsetsStruct FirstPartition = {1234, 12345, 0};
    OffsetsStruct SecondPartition{2234, 22345, 1};
    OffsetsStruct ThirdPartition = getPartitionHighAndLowOffsets(Topic, 3);
    VectorOfPartitions.push_back(FirstPartition);
    VectorOfPartitions.push_back(SecondPartition);
    VectorOfPartitions.push_back(ThirdPartition);
  }
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
