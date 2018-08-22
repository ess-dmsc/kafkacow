#include "../src/ConnectKafkaFakeTestClass.h"
#include "../src/ConnectKafkaParentClass.h"
#include "../src/RequestHandler.h"
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  std::string Message =
      NewRequestHandler.SubscribeConsumeNLastMessages("ExampleTestTopic", 100);

  EXPECT_EQ("HiddenSecretMessageFromLovingNeutron", Message);
}

TEST(RequestHandlerTest, subscribe_at_an_offset_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  std::string Message =
      NewRequestHandler.SubscribeConsumeAtOffset("ExampleTestTopic", 100);

  EXPECT_EQ("HiddenSecretMessageFromLovingNeutron", Message);
}

TEST(RequestHandlerTest, topic_metadata_creation_test) {
  TopicMetadataStruct TopicMetadataStructTest;
  TopicMetadataStructTest.Name = "TestName";
  std::vector<int32_t> PartitionVector;
  PartitionVector.assign(3, 4);
  TopicMetadataStructTest.Partitions.assign(3, 4);

  EXPECT_EQ(PartitionVector, TopicMetadataStructTest.Partitions);
  EXPECT_EQ("TestName", TopicMetadataStructTest.Name);
}