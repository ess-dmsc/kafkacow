#include "../src/ArgumentsException.h"
#include "../src/ConnectKafkaFake.h"
#include "../src/ConnectKafkaInterface.h"
#include "../src/FlatbuffersTranslator.h"
#include "../src/RequestHandler.h"
#include <boost/filesystem.hpp>
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeConsumeNLastMessages("ExampleTestTopic", 100));
}

TEST(RequestHandlerTest, subscribe_at_an_offset_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeConsumeAtOffset("ExampleTestTopic", 100));
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

TEST(RequestHandlerTest, checkandrun_consumer_mode_chosen_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.OffsetToStart = 1234;
  UserArguments.Name = "TestTopicName";

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun(UserArguments));
}

TEST(RequestHandlerTest, checkandrun_metadata_mode_chosen_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.MetadataMode = true;
  UserArguments.ShowAllTopics = true;

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun(UserArguments));
}

TEST(RequestHandlerTest, error_thrown_if_no_mode_specified) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = false;
  UserArguments.MetadataMode = false;

  EXPECT_THROW(NewRequestHandler.checkAndRun(UserArguments),
               ArgumentsException);
}

// metadata mode arguments test
TEST(RequestHandlerTest, show_topic_partition_offsets_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowPartitionsOffsets = true;
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}
TEST(RequestHandlerTest, show_all_topics_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = true;
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}
TEST(RequestHandlerTest, no_action_specified_in_metadata_mode) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = false;
  UserArguments.ShowPartitionsOffsets = false;
  EXPECT_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments),
               ArgumentsException);
}

// consumer mode argument test

TEST(RequestHandlerTest, both_goback_and_offsettostart_specified_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1234;
  UserArguments.GoBack = 1234;
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments),
               ArgumentsException);
}

TEST(RequestHandlerTest, subscribe_to_nlastmessages_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = -1234;
  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments));
}

TEST(RequestHandlerTest, use_what_message_of_arguments_exception) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.MetadataMode = true;

  std::string message;
  try {
    NewRequestHandler.checkAndRun(UserArguments);
  } catch (ArgumentsException &exception) {
    message = exception.what();
  }
  EXPECT_EQ(
      message,
      "Program can run in one and only one mode: --consumer or --metadata");
}
