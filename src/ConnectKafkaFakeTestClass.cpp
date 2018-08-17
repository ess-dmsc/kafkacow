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
