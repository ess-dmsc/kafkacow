#include "../src/ArgumentsException.h"
#include "../src/ConnectKafkaFake.h"
#include "../src/ConnectKafkaInterface.h"
#include "../src/FlatbuffersTranslator.h"
#include "../src/RequestHandler.h"
#include <boost/filesystem.hpp>
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest,
     subscribe_consume_n_last_messages_throws_if_incorrect_arguments_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  EXPECT_THROW(NewRequestHandler.subscribeConsumeNLastMessages(
                   "ExampleTestTopic", 100, 1),
               ArgumentsException);
}

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_successful_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  EXPECT_NO_THROW(NewRequestHandler.subscribeConsumeNLastMessages(
      "ExampleTestTopic", 1, 1));
}

TEST(RequestHandlerTest,
     subscribe_at_an_offset_throws_if_incorrect_arguments_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  EXPECT_THROW(
      NewRequestHandler.subscribeConsumeAtOffset("ExampleTestTopic", 100),
      ArgumentsException);
}

TEST(RequestHandlerTest, subscribe_at_an_offset_successful_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeConsumeAtOffset("ExampleTestTopic", 12344));
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

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.OffsetToStart = 1235;
  UserArguments.Name = "TestTopicName";
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, checkandrun_metadata_mode_chosen_test) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.MetadataMode = true;
  UserArguments.ShowAllTopics = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, error_thrown_if_no_mode_specified) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = false;
  UserArguments.MetadataMode = false;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_THROW(NewRequestHandler.checkAndRun(), ArgumentsException);
}

// metadata mode arguments test
TEST(RequestHandlerTest, show_topic_partition_offsets_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.Name = "MULTIPART_events";
  UserArguments.ConsumerMode = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}
TEST(RequestHandlerTest, show_all_topics_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments(UserArguments));
}

TEST(RequestHandlerTest, display_all_metadata) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = false;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  testing::internal::CaptureStdout();
  NewRequestHandler.checkMetadataModeArguments(UserArguments);
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_EQ("Test return", OutputMessage);
}

// consumer mode argument test

TEST(RequestHandlerTest, both_goback_and_offsettostart_specified_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1234;
  UserArguments.GoBack = 1234;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments),
               ArgumentsException);
}

TEST(RequestHandlerTest, subscribe_to_nlastmessages_no_error) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = -1234;
  UserArguments.PartitionToConsume = 1;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);
  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments(UserArguments));
}

TEST(RequestHandlerTest, use_what_message_of_arguments_exception) {
  auto KafkaConnection = std::make_unique<ConnectKafkaFake>(ConnectKafkaFake());

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.MetadataMode = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments);

  std::string message;
  try {
    NewRequestHandler.checkAndRun();
  } catch (ArgumentsException &exception) {
    message = exception.what();
  }
  EXPECT_EQ(
      message,
      "Program can run in one and only one mode: --consumer or --metadata");
}
