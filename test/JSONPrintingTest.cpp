#include "../src/JSONPrinting.h"
#include "f142_logdata_generated.h"
#include <gtest/gtest.h>

bool CompactOFF{false};
bool CompactON{true};

class JSONPrintingTest : public ::testing::Test {};

TEST(JSONPrintingTest, print_entire_message_test) {
  std::string InputMessage = "{\n  \"source_name\": \"NeXus-Streamer\"}";
  EXPECT_EQ(getEntireMessage(InputMessage, 4), "{\n    source_name: "
                                               "NeXus-Streamer\n}");
}

TEST(JSONPrintingTest, print_truncated_array_message_test) {
  std::string InputMessage =
      "{\"time_of_flight\": [\n    15579,\n    91072,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "37233\n]}";
  EXPECT_EQ(getTruncatedMessage(InputMessage, 4, CompactOFF),
            "{\n    time_of_flight: [\n        15579\n        91072\n        "
            "32972\n        79344\n        22827\n        32972\n        "
            "79344\n        22827\n        32972\n        79344\n        "
            "...\n        Truncated 5 elements.\n    "
            "]\n}");
}

TEST(JSONPrintingTest, print_compacted_array_message_test) {
  std::string InputMessage =
      "{\"time_of_flight\": [\n    15579,\n    91072,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "32972,\n    79344,\n    22827,\n    32972,\n    79344,\n    22827,\n    "
      "37233\n]}";
  EXPECT_EQ(getTruncatedMessage(InputMessage, 1, CompactON),
            "{\n time_of_flight: [   15579   91072   32972   "
            "... truncated 12 elements ...   37233  ]\n}");
}

TEST(JSONPrintingTest, print_truncated_string_message_test) {
  std::string InputMessage =
      R"({"long_string": "This is a long string which should be truncated by kafkacow"})";
  EXPECT_EQ(getTruncatedMessage(InputMessage, 4, CompactOFF),
            "{\n    long_string: This is a long string which should be "
            "truncated by ... Truncated 9 characters.\n}");
}

TEST(JSONPrintingTest, print_nested_maps_and_sequences_test) {
  std::string InputMessage = "[ 15579, 91072, {\n"
                             "  \"32972\": {\n"
                             "    \"32972\":\n"
                             "    32972}},[15579, 91072] ]";
  EXPECT_NO_THROW(getTruncatedMessage(InputMessage, 4, CompactOFF));
}

TEST(JSONPrintingTest, print_empty_arrays_empty_maps) {
  std::string InputMessage = "{"
                             "    \"emptyArray\":[],"
                             "    \"emptyObject\": {}"
                             "}";
  EXPECT_NO_THROW(getTruncatedMessage(InputMessage, 4, CompactOFF));
}

TEST(JSONPrintingTest, print_entire_message_empty_arrays_empty_maps) {
  std::string InputMessage = "{"
                             "    \"emptyArray\":[],"
                             "    \"emptyObject\": {}"
                             "}";
  EXPECT_NO_THROW(getEntireMessage(InputMessage, 4));
}

TEST(JSONPrintingTest, print_entire_non_json) {
  std::string InputMessage =
      "This is not a json message and it should be displayed without parsing";
  EXPECT_EQ(getEntireMessage(InputMessage, 4), InputMessage);
}

TEST(JSONPrintingTest, print_truncated_non_json) {
  std::string LongMessage = "This is a message that should be long enough to "
                            "truncate, and it is not JSON.";
  EXPECT_TRUE(LongMessage.find(getTruncatedMessage(
                  LongMessage, 4, CompactOFF)) != std::string::npos);
}

TEST(JSONPrintingTest, print_non_json_too_short_to_truncate) {
  std::string ShortMessage = "Too short";
  EXPECT_TRUE(ShortMessage.find(getTruncatedMessage(
                  ShortMessage, 4, CompactOFF)) != std::string::npos);
}
