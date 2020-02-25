#include "FakeConsumer.h"

namespace Kafka {

std::string FakeConsumer::getAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

MessageMetadataStruct FakeConsumer::consume() {
  MessageMetadataStruct DataToReturn;
  DataToReturn.Payload = "\"HiddenSecretMessageFromLovingNeutron\"";
  DataToReturn.PartitionEOF = true;
  DataToReturn.Timestamp = 1542;
  DataToReturn.Offset = 1234;
  DataToReturn.Partition = 0;
  DataToReturn.KeyPresent = true;
  DataToReturn.Key = "MessageKey";
  return DataToReturn;
}

std::vector<Metadata::Partition>
FakeConsumer::getTopicsHighAndLowOffsets(const std::string &Topic) {

  std::vector<Metadata::Partition> VectorOfPartitions;

  if (Topic != "EmptyTopic") {
    Metadata::Partition FirstPartition = {1234, 12345, 0};
    Metadata::Partition SecondPartition{2234, 22345, 1};
    Metadata::Partition ThirdPartition =
        getPartitionHighAndLowOffsets(Topic, 3);
    VectorOfPartitions.push_back(FirstPartition);
    VectorOfPartitions.push_back(SecondPartition);
    VectorOfPartitions.push_back(ThirdPartition);
  }
  return VectorOfPartitions;
}

int FakeConsumer::getNumberOfTopicPartitions(const std::string &TopicName) {
  return 1;
}

void FakeConsumer::subscribeAtOffset(int64_t Offset, const std::string &Topic) {
}

void FakeConsumer::subscribeToLastNMessages(int64_t NMessages,
                                            const std::string &TopicName,
                                            int Partition) {}

std::string FakeConsumer::showAllMetadata() { return "Test return"; }

Metadata::Partition
FakeConsumer::getPartitionHighAndLowOffsets(const std::string &Topic,
                                            int32_t PartitionID) {
  Metadata::Partition OffsetsToReturn{1, 5, PartitionID};
  return OffsetsToReturn;
}

int64_t FakeConsumer::getOffsetForDate(const std::string &Date,
                                       const std::string &Topic) {
  return 1;
}
}
