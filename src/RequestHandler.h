#pragma once

#include "ArgumentsException.h"
#include "ConnectKafkaInterface.h"
#include "FlatbuffersTranslator.h"
#include "UserArgumentsStruct.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler {
public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaInterface> KafkaConnection,
      UserArgumentStruct &UserArguments, std::string SchemaPath)
      : KafkaConnection(std::move(KafkaConnection)),
        SchemaPath(std::move(SchemaPath)) {
    Logger = spdlog::get("LOG");
    this->UserArguments = UserArguments;
  }

  void checkAndRun();

  void checkConsumerModeArguments(UserArgumentStruct UserArguments);

  void checkMetadataModeArguments(UserArgumentStruct UserArguments);

  void showTopicPartitionOffsets(UserArgumentStruct UserArguments);

  void subscribeConsumeAtOffset(std::string TopicName, int64_t Offset);
  void subscribeConsumeNLastMessages(std::string TopicName,
                                     int64_t NumberOfMessages, int Partition);

  void subscribeConsumeRange(const int64_t &Offset,
                             const int64_t &NumberOfMessages,
                             const int &Partition,
                             const std::string &TopicName);

private:
  std::shared_ptr<spdlog::logger> Logger;
  UserArgumentStruct UserArguments;
  std::unique_ptr<ConnectKafkaInterface> KafkaConnection;
  const std::string SchemaPath;
  
  void consumePartitions(KafkaMessageMetadataStruct &MessageData,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);
  bool verifyOffset(const int64_t Offset, const std::string TopicName);
  void verifyNLast(const int64_t NLast, const std::string TopicName,
                   const int16_t Partition);
  void printMessageMetadata(KafkaMessageMetadataStruct &MessageData);
  void printEntireTopic(const std::string &TopicName);
  void checkIfTopicEmpty(const std::string &TopicName);
};
