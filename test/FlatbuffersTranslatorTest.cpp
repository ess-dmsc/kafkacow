#include "../src/FlatbuffersTranslator.h"
#include "SchemaPathForTest.h"
#include "f142_logdata_generated.h"
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

class FlatbuffersTranslatorTest : public ::testing::Test {

public:
  static std::string getStringToCompare(const std::string &Source,
                                        const std::string &Value,
                                        const std::string &TimeStamp) {
    std::string ToCompare = R"({
  source_name: ")";
    ToCompare.append(Source);
    ToCompare.append(R"(",
  value_type: "Int",
  value: {
    value: )");
    ToCompare.append(Value);
    ToCompare.append(R"(
  },
  timestamp: )");
    ToCompare.append(TimeStamp);
    ToCompare.append(R"(
}
)");
    return ToCompare;
  }
};

TEST(FlatbuffersTranslatorTest, translate_flatbuffers_test) {
  flatbuffers::FlatBufferBuilder Builder;
  // flattbuffer values
  auto SourceName = Builder.CreateString("SourceName");
  auto ValueOffset = CreateInt(Builder, 11111);
  uint64_t Timestamp = 22222222222;
  // strings to compare
  std::string SourceNameCompare = "SourceName";
  std::string ValueCompare = "11111";
  std::string TimeStampCompare = "22222222222";

  auto Message = CreateLogData(Builder, SourceName, Value_Int,
                               ValueOffset.Union(), Timestamp);
  FinishLogDataBuffer(Builder, Message);
  auto bufferpointer =
      reinterpret_cast<const char *>(Builder.GetBufferPointer());
  std::string NewMessage(bufferpointer, bufferpointer + Builder.GetSize());
  auto FlatbufferPointer = Builder.ReleaseBufferPointer();

  KafkaMessageMetadataStruct MessageMetadata;
  MessageMetadata.Payload = NewMessage;
  FlatbuffersTranslator FlatBuffersTranslator(getSchemaPathForTest());

  // Run first time to populate schema map
  std::string FileID;
  FlatBuffersTranslator.deserializeToYAML(MessageMetadata, FileID);
  EXPECT_EQ(FlatBuffersTranslator.deserializeToYAML(MessageMetadata, FileID),
            FlatbuffersTranslatorTest::getStringToCompare(
                SourceNameCompare, ValueCompare, TimeStampCompare));
  EXPECT_EQ(FileID, "f142");
}

TEST(FlatbuffersTranslatorTest, message_already_in_json_test) {
  KafkaMessageMetadataStruct MessageMetadata;
  MessageMetadata.Payload = "{\n  source_name: \"NeXus-Streamer\"}";
  FlatbuffersTranslator FlatBuffersTranslator(getSchemaPathForTest());
  std::string FileID;
  EXPECT_EQ(FlatBuffersTranslator.deserializeToYAML(MessageMetadata, FileID),
            MessageMetadata.Payload);
}

TEST(FlatbuffersTranslatorTest,
     no_throw_for_short_messages_without_file_identifier) {
  FlatbuffersTranslator FlatBuffersTranslator(getSchemaPathForTest());
  std::string FileID;
  KafkaMessageMetadataStruct MessageMetadata;
  MessageMetadata.Payload = "test";
  EXPECT_NO_THROW(
      FlatBuffersTranslator.deserializeToYAML(MessageMetadata, FileID));
}
