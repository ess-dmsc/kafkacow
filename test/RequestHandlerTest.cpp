#include "../src/ConnectKafkaFake.h"
#include "../src/ConnectKafkaInterface.h"
#include "../src/RequestHandler.h"
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  std::string Message =
      NewRequestHandler.subscribeConsumeNLastMessages("ExampleTestTopic", 100);

  EXPECT_EQ("HiddenSecretMessageFromLovingNeutron", Message);
}

TEST(RequestHandlerTest, subscribe_at_an_offset_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  std::string Message =
      NewRequestHandler.subscribeConsumeAtOffset("ExampleTestTopic", 100);

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

// successful run from beginning to end
TEST(RequestHandlerTest, init_method_successful_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.MetadataMode = true;
  UserArguments.ShowPartitionsOffsets = true;

  EXPECT_EQ(NewRequestHandler.init(UserArguments), 0);
}

TEST(RequestHandlerTest, checkandrun_consumer_mode_chosen_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.OffsetToStart = 1234;
  UserArguments.Name = "TestTopicName";

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun(UserArguments));
}

TEST(RequestHandlerTest, checkandrun_no_mode_selected_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = false;
  UserArguments.MetadataMode = false;

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun(UserArguments));
}

// metadata mode arguments test
TEST(RequestHandlerTest,
     check_metadata_mode_arguments_successful_if_branch_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowPartitionsOffsets = true;
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}
TEST(RequestHandlerTest,
     check_metadata_mode_arguments_showalltopics_if_branch_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = true;
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}
TEST(RequestHandlerTest,
     check_metadata_mode_arguments_noactionspecified_if_branch_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = false;
  UserArguments.ShowPartitionsOffsets = false;
  EXPECT_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments),
               ArgumentsException);
}

// consumer mode argument test

TEST(RequestHandlerTest,
     consumer_mode_arguments_check_offsets_and_lastN_specified_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1234;
  UserArguments.GoBack = 1234;
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments),
               ArgumentsException);
}

TEST(RequestHandlerTest,
     consumer_mode_arguments_check_nlastmessages_branch_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = -1234;
  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments));
}
