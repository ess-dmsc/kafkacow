#include "ConnectKafkaFakeTestClass.h"

std::unique_ptr<RdKafka::Metadata> ConnectKafkaFakeTestClass::queryMetadata() {
  return nullptr;
}

ConnectKafkaFakeTestClass::ConnectKafkaFakeTestClass() {}

std::string ConnectKafkaFakeTestClass::GetAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

void ConnectKafkaFakeTestClass::SubscribeToTopic(
    const std::vector<std::string> &Topic) {}

bool ConnectKafkaFakeTestClass::CheckIfTopicExists(std::string Topic) {
  return Topic.compare("Topic that exists") == 0;
}

std::string ConnectKafkaFakeTestClass::Consume(std::string Topic) {
  return "Subscribed";
}

TopicMetadataStruct
ConnectKafkaFakeTestClass::GetTopicMetadata(std::string Topic) {
  TopicMetadataStruct MetadataStruct;
  return MetadataStruct;
}

std::unique_ptr<int64_t> ConnectKafkaFakeTestClass::GetCurrentPartitionOffset(
    const RdKafka::TopicMetadata::PartitionMetadataVector *) {
  return std::unique_ptr<int64_t>();
}

std::vector<OffsetsStruct>
ConnectKafkaFakeTestClass::GetHighAndLowOffsets(std::string Topic) {
  std::vector<OffsetsStruct> VectorOfPartitions;
  OffsetsStruct FirstPartition;
  FirstPartition.PartitionId = 0;
  FirstPartition.HighOffset = 12345;
  FirstPartition.LowOffset = 1234;
  OffsetsStruct SecondPartition;
  SecondPartition.PartitionId = 1;
  SecondPartition.HighOffset = 22345;
  SecondPartition.LowOffset = 2234;
  VectorOfPartitions.push_back(FirstPartition);
  VectorOfPartitions.push_back(SecondPartition);
  return VectorOfPartitions;
}

std::vector<int32_t>
ConnectKafkaFakeTestClass::GetTopicPartitions(std::string Topic) {
  return std::vector<int32_t>();
}
