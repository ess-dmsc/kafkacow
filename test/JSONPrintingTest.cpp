#include "../src/FlatbuffersTranslator.h"
#include "../src/JSONPrinting.h"
#include "f142_logdata_generated.h"
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

class JSONPrintingTest : public ::testing::Test {};

TEST(JSONPrintingTest, print_entire_message_test) {
  std::string InputMessage = "{\n  source_name: \"NeXus-Streamer\"}";
  EXPECT_EQ(getEntireMessage(InputMessage), "{\n    source_name: "
                                            "NeXus-Streamer\n}");
}

TEST(JSONPrintingTest, print_truncated_message_test) {
  std::string InputMessage =
      "{time_of_flight: [\n    15579,\n    91072,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "37233,\n]}";
  EXPECT_EQ(getTruncatedMessage(InputMessage),
            "{\n    time_of_flight: [\n        15579\n        91072\n        "
            "32972\n        79344\n        22827\n        32972\n        "
            "79344\n        22827\n        32972\n        79344\n        "
            "[...]\n        Omitted 5 results.\n    "
            "]\n}");
}

TEST(JSONPrintingTest, print_message_test) {

  testing::internal::CaptureStdout();
  printToScreen("TestMessage");
  std::string OutputMessage = testing::internal::GetCapturedStdout();
  EXPECT_EQ(
      OutputMessage,
      "TestMessage\n__________________________________________________\n");
}