#pragma once

#include "ArgumentsException.h"
#include "FlatbuffersTranslator.h"
#include "RequestHandlerInterface.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler : public RequestHandlerInterface {
public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaInterface> KafkaConnection,
      UserArgumentStruct &UserArguments)
      : KafkaConnection(std::move(KafkaConnection)) {
    Logger = spdlog::get("LOG");
    this->UserArguments = UserArguments;
  }

  void checkAndRun() override;

  void checkConsumerModeArguments(UserArgumentStruct UserArguments) override;

  void checkMetadataModeArguments(UserArgumentStruct UserArguments) override;

  void showTopicPartitionOffsets(UserArgumentStruct UserArguments) override;

  void subscribeConsumeAtOffset(std::string TopicName, int32_t Offset) override;
  void subscribeConsumeNLastMessages(std::string TopicName,
                                     int64_t NumberOfMessages,
                                     int Partition) override;

private:
  std::unique_ptr<ConnectKafkaInterface> KafkaConnection;
  void consumePartitions(KafkaMessageMetadataStruct &MessageData,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);
  void verifyOffset(int32_t Offset, std::string TopicName);
  void verifyNLast(int64_t NLast, std::string TopicName, int16_t Partition);
  std::shared_ptr<spdlog::logger> Logger;
  UserArgumentStruct UserArguments;
};
