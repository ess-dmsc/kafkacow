#include "ConnectKafkaFakeTestClass.h"

std::unique_ptr<RdKafka::Metadata> ConnectKafkaFakeTestClass::queryMetadata() {
  return nullptr;
}

ConnectKafkaFakeTestClass::ConnectKafkaFakeTestClass() {}

std::string ConnectKafkaFakeTestClass::GetAllTopics() {
  return "Successful test 1\nSuccessful test 2";
}

std::string ConnectKafkaFakeTestClass::SubscribeToTopic(
    const std::vector<std::string> &Topic) {
  return nullptr;
}

bool ConnectKafkaFakeTestClass::CheckIfTopicExists(std::string Topic) {
  return Topic.compare("Topic that exists") == 0;
}
