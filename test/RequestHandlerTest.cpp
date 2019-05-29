#include "../src/CustomExceptions.h"
#include "../src/FlatbuffersTranslator.h"
#include "../src/KafkaW/ConsumerInterface.h"
#include "../src/KafkaW/FakeConsumer.h"
#include "../src/KafkaW/FakeProducer.h"
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
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_THROW(NewRequestHandler.checkAndRun(), ArgumentException);
}

TEST(RequestHandlerTest, subscribe_consume_n_last_messages_successful_test) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(
      NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 1, 1));
}

TEST(RequestHandlerTest,
     subscribe_at_an_offset_throws_if_incorrect_arguments_test) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_THROW(NewRequestHandler.subscribeAndConsume("ExampleTestTopic", 100),
               ArgumentException);
}

TEST(RequestHandlerTest, subscribe_at_an_offset_successful_test) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

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
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.OffsetToStart = 1235;
  UserArguments.GoBack = 1;
  UserArguments.TopicName = "TestTopicName";
  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, checkandrun_metadata_mode_chosen_test) {
  UserArgumentStruct UserArguments;
  UserArguments.MetadataMode = true;
  UserArguments.ShowAllTopics = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, error_thrown_if_no_mode_specified) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = false;
  UserArguments.MetadataMode = false;
  EXPECT_THROW(RequestHandler NewRequestHandler(UserArguments,
                                                updateSchemas(false), false),
               ArgumentException);
}

// metadata mode arguments test
TEST(RequestHandlerTest, show_topic_partition_offsets_no_error) {
  UserArgumentStruct UserArguments;
  UserArguments.TopicName = "MULTIPART_events";
  UserArguments.ConsumerMode = true;
  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments());
}
TEST(RequestHandlerTest, show_all_topics_no_error) {
  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = true;
  UserArguments.MetadataMode = true;
  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(NewRequestHandler.checkMetadataModeArguments());
}

TEST(RequestHandlerTest, display_all_metadata) {
  UserArgumentStruct UserArguments;
  UserArguments.ShowAllTopics = false;
  UserArguments.MetadataMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  testing::internal::CaptureStdout();
  NewRequestHandler.checkMetadataModeArguments();
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_EQ("Test return", OutputMessage);
}

// consumer mode argument test

TEST(RequestHandlerTest,
     display_range_when_both_goback_and_offsettostart_specified) {
  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1234;
  UserArguments.GoBack = 2;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments());
}

TEST(RequestHandlerTest, subscribe_to_nlastmessages_no_error) {
  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = -1234;
  UserArguments.PartitionToConsume = 1;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);

  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments());
}

TEST(RequestHandlerTest, use_what_message_of_arguments_exception) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;
  UserArguments.MetadataMode = true;

  std::string message;
  try {
    RequestHandler NewRequestHandler(UserArguments, updateSchemas(false),
                                     false);
  } catch (ArgumentException &exception) {
    message = exception.what();
  }
  EXPECT_EQ(message, "Program can run in one and only one mode: --consumer, "
                     "--metadata or --producer");
}

TEST(RequestHandlerTest, throw_error_when_lower_range_bound_incorrect) {
  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 1233;
  UserArguments.GoBack = 2;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_when_upper_range_bound_incorrect) {
  UserArgumentStruct UserArguments;
  UserArguments.OffsetToStart = 22343;
  UserArguments.GoBack = 5;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_no_topic_specified_in_consumer_mode) {
  UserArgumentStruct UserArguments;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, throw_error_if_topic_empty) {
  UserArgumentStruct UserArguments;
  UserArguments.TopicName = "EmptyTopic";
  UserArguments.GoBack = 5;
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_THROW(NewRequestHandler.checkConsumerModeArguments(),
               ArgumentException);
}

TEST(RequestHandlerTest, print_entire_topic_success) {
  UserArgumentStruct UserArguments;
  UserArguments.TopicName = "TestTopic";
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_NO_THROW(NewRequestHandler.checkConsumerModeArguments(true));
}

TEST(RequestHandlerTest, display_message_metadata_with_message_key) {
  UserArgumentStruct UserArguments;
  UserArguments.TopicName = "TestTopic";
  UserArguments.ConsumerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  testing::internal::CaptureStdout();
  NewRequestHandler.checkConsumerModeArguments(true);
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(OutputMessage.find("Key: MessageKey") != std::string::npos);
}

// PRODUCER

TEST(RequestHandlerTest, run_producer) {
  UserArgumentStruct UserArguments;
  UserArguments.ProducerMode = true;

  RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
  EXPECT_NO_THROW(NewRequestHandler.checkAndRun());
}

TEST(RequestHandlerTest, throw_error_if_file_specified_in_consumer_mode) {
    UserArgumentStruct UserArguments;
    UserArguments.ConsumerMode = true;
    UserArguments.JSONPath = "Path/to.json";

    RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
    EXPECT_THROW(NewRequestHandler.checkAndRun(),ArgumentException);
}

TEST(RequestHandlerTest, throw_error_if_file_specified_in_metadata_mode) {
    UserArgumentStruct UserArguments;
    UserArguments.MetadataMode = true;
    UserArguments.JSONPath = "Path/to.json";

    RequestHandler NewRequestHandler(UserArguments, updateSchemas(false), false);
    EXPECT_THROW(NewRequestHandler.checkAndRun(),ArgumentException);
}
