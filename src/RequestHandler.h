#pragma once

#include "ConnectKafkaInterface.h"
#include "CustomExceptions.h"
#include "FlatbuffersTranslator.h"
#include "UserArgumentsStruct.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler {
public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaInterface> KafkaConnection,
      UserArgumentStruct &UserArguments)
      : KafkaConnection(std::move(KafkaConnection)) {
    Logger = spdlog::get("LOG");
    this->UserArguments = UserArguments;
  }

  void checkAndRun();

  void checkConsumerModeArguments(UserArgumentStruct UserArguments);

  void checkMetadataModeArguments(UserArgumentStruct UserArguments);

  void showTopicPartitionOffsets(UserArgumentStruct UserArguments);

  void subscribeAndConsume(std::string TopicName, int64_t Offset);
  void subscribeAndConsume(std::string TopicName, int64_t NumberOfMessages,
                           int Partition);

  void subscribeAndConsume(const std::string &TopicName,
                           const int64_t &NumberOfMessages,
                           const int &Partition, const int64_t &Offset);

private:
  std::shared_ptr<spdlog::logger> Logger;
  UserArgumentStruct UserArguments;
  std::unique_ptr<ConnectKafkaInterface> KafkaConnection;
  void printKafkaMessage(KafkaMessageMetadataStruct &MessageData,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);
  bool verifyOffset(int64_t Offset, const std::string &TopicName);
  void verifyNLast(int64_t NLast, const std::string &TopicName,
                   int16_t Partition);
  void printMessageMetadata(KafkaMessageMetadataStruct &MessageData,
                            const std::string &FileIdentifier);
  void printEntireTopic(const std::string &TopicName);
  void checkIfTopicEmpty(const std::string &TopicName);
  std::string timestampToReadable(const int64_t &Timestamp);
  void consumeSingleMessage(int &EOFPartitionCounter,
                            FlatbuffersTranslator &FlatBuffers);
};
