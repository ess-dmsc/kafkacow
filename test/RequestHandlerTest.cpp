#include "../src/ConnectKafkaFakeTestClass.h"
#include "../src/ConnectKafkaParentClass.h"
#include "../src/RequestHandler.h"
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest, get_all_topics_method_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));
  EXPECT_EQ(NewRequestHandler.GetAllTopics(),
            "Successful test 1\nSuccessful test 2");
}

TEST(RequestHandlerTest, print_method_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  NewRequestHandler.PrintToScreen("String to print");
}

TEST(RequestHandlerTest, check_if_topic_exists_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  EXPECT_TRUE(NewRequestHandler.CheckIfTopicExists("Topic that exists"));
}

TEST(RequestHandlerTest, check_if_topic_does_not_exist_test) {
  auto KafkaConnection =
      std::make_unique<ConnectKafkaFakeTestClass>(ConnectKafkaFakeTestClass());
  RequestHandler NewRequestHandler(std::move(KafkaConnection));

  EXPECT_FALSE(
      NewRequestHandler.CheckIfTopicExists("Topic that does not exist"));
}