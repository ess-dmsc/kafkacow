#include "../src/CustomExceptions.h"
#include "../src/FlatbuffersTranslator.h"
#include "../src/KafkaW/FakeConsumer.h"
#include "../src/KafkaW/ConsumerInterface.h"
#include "../src/RequestHandler.h"
#include "../src/UpdateSchemas.h"
#include <boost/filesystem.hpp>
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

namespace {
const std::string SchemaPath = "schemas/";
}

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest,
     subscribe_consume_n_last_messages_throws_if_incorrect_arguments_test) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_THROW(
      NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 100, 1),
      ArgumentException);
}

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_successful_test) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 1, 1));
}

TEST(RequestHandlerTest,
     subscribe_at_an_offset_throws_if_incorrect_arguments_test) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_THROW(NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 100),
               ArgumentException);
}

TEST(RequestHandlerTest, subscribe_at_an_offset_successful_test) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());
  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 12344, true));
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
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.OffsetToStart = 1235;
  UserArguments.GoBack = 1;
  UserArguments.Name = "TestTopicName";
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, checkandrun_metadata_mode_chosen_test) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.MetadataMode = true;
  UserArguments.ShowAllTopics = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, error_thrown_if_no_mode_specified) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = false;
  UserArguments.MetadataMode = false;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_THROW(NewRequestHandler.checkAndRun(), ArgumentException);
}

// metadata mode arguments test
TEST(RequestHandlerTest, show_topic_partition_offsets_no_error) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.Name = "MULTIPART_events";
  UserArguments.ConsumerMode = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments());
}
TEST(RequestHandlerTest, show_all_topics_no_error) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments());
}

TEST(RequestHandlerTest, display_all_metadata) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = false;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  testing::internal::CaptureStdout();
  NewRequestHandler.checkMetadataModeArguments();
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_EQ("Test return", OutputMessage);
}

// consumer mode argument test

TEST(RequestHandlerTest,
     display_range_when_both_goback_and_offsettostart_specified) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1234;
  UserArguments.GoBack = 2;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments());
}

TEST(RequestHandlerTest, subscribe_to_nlastmessages_no_error) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = -1234;
  UserArguments.PartitionToConsume = 1;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments());
}

TEST(RequestHandlerTest, use_what_message_of_arguments_exception) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.MetadataMode = true;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));

  std::string message;
  try {
    NewRequestHandler.checkAndRun();
  } catch (ArgumentException &exception) {
    message = exception.what();
  }
  EXPECT_EQ(
      message,
      "Program can run in one and only one mode: --consumer or --metadata");
}

TEST(RequestHandlerTest, throw_error_when_lower_range_bound_incorrect) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1233;
  UserArguments.GoBack = 2;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_when_upper_range_bound_incorrect) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 22343;
  UserArguments.GoBack = 5;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_no_topic_specified_in_consumer_mode) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_if_topic_empty) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.Name = "EmptyTopic";
  UserArguments.GoBack = 5;
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, print_entire_topic_success) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.Name = "TestTopic";
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments(true));
}

TEST(RequestHandlerTest, display_message_metadata_with_message_key) {
  auto KafkaConnection = std::make_unique<FakeConsumer>(FakeConsumer());

  UserArgumentStruct UserArguments;
  UserArguments.Name = "TestTopic";
  RequestHandler NewRequestHandler(std::move(KafkaConnection), UserArguments,
                                   updateSchemas(false));
  testing::internal::CaptureStdout();
  NewRequestHandler.checkConsumerModeArguments(true);
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(OutputMessage.find("Key: MessageKey") != std::string::npos);
}
