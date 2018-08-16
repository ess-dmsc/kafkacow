#include "../src/RequestHandler.h"
#include "ConnectKafkaFakeTestClass.h"
#include <gtest/gtest.h>

class RequestHandlerTest : public ::testing::Test {};

TEST(RequestHandlerTest, first_test) { EXPECT_EQ(1, 1); }

TEST(RequestHandlerTest, get_all_topics_method_test) {}