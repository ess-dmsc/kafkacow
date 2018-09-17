#include "../src/FlatbuffersTranslator.h"
#include "f142_logdata_generated.h"
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>

class FlatbuffersTranslatorTest : public ::testing::Test {

public:
  static std::string getStringToCompare(std::string Source, std::string Value,
                                        std::string TimeStamp) {
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
  FlatbuffersTranslator FlatBuffersTranslator;

  // Run first time to populate schema map
  FlatBuffersTranslator.translateToJSON(MessageMetadata);
  EXPECT_EQ(FlatBuffersTranslator.translateToJSON(MessageMetadata),
            FlatbuffersTranslatorTest::getStringToCompare(
                SourceNameCompare, ValueCompare, TimeStampCompare));
}
