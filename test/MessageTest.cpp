#include "../src/Kafka/Message.h"
#include <flatbuffers/idl.h>
#include <gtest/gtest.h>
#include <json_json_generated.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

class MessageTest : public ::testing::Test {};

TEST(MessageTest, message_struct_methods_working_correctly) {

  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();
  std::string MessageToSerialize = "{\"SimpleJson\" : 42}";
  auto FBOffset = CreateJsonDataDirect(builder, MessageToSerialize.c_str());
  FinishJsonDataBuffer(builder, FBOffset);
  auto KafkaMessage = Kafka::Message(builder.Release());
  std::string MessageString(KafkaMessage.data(),
                            static_cast<int>(KafkaMessage.size()));
  EXPECT_EQ(GetJsonData(MessageString.c_str())->json()->str(),
            MessageToSerialize);
  EXPECT_EQ(static_cast<int>(KafkaMessage.size()), MessageString.size());
}