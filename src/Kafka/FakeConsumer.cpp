#include "FakeConsumer.h"

namespace Kafka {

FakeConsumer::FakeConsumer() {}

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

std::vector<OffsetsStruct>
FakeConsumer::getTopicsHighAndLowOffsets(const std::string &Topic) {

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

int FakeConsumer::getNumberOfTopicPartitions(std::string TopicName) {
  return 1;
}

void FakeConsumer::subscribeAtOffset(int64_t Offset, std::string TopicName) {}

void FakeConsumer::subscribeToLastNMessages(int64_t NMessages,
                                            const std::string &TopicName,
                                            int Partition) {}

std::string FakeConsumer::showAllMetadata() { return "Test return"; }

OffsetsStruct
FakeConsumer::getPartitionHighAndLowOffsets(const std::string &Topic,
                                            int32_t PartitionID) {
  OffsetsStruct OffsetsToReturn{1, 5, PartitionID};
  return OffsetsToReturn;
}

int64_t FakeConsumer::getOffsetForDate(const std::string &Date,
                                       const std::string &Topic) {
  return 1;
}
}
