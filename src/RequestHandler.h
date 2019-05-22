#pragma once

#include "CustomExceptions.h"
#include "FlatbuffersTranslator.h"
#include "KafkaW/ConsumerInterface.h"
#include "KafkaW/Producer.h"
#include "UserArgumentsStruct.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler {
public:
  explicit RequestHandler(UserArgumentStruct &UserArguments,
                          std::string FullSchemaPath, bool Real = true)
      : Real(Real), Logger(spdlog::get("LOG")), UserArguments(UserArguments),
        SchemaPath(std::move(FullSchemaPath)) {}

  void checkAndRun();

  void checkConsumerModeArguments(bool TerminateAtEndOfTopic = false);

  void checkMetadataModeArguments();

  void checkProducerModeArguments();

  void showTopicPartitionOffsets();

  void subscribeAndConsume(const std::string &TopicName, int64_t Offset,
                           bool TerminateAtEndOfTopic = false);
  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int Partition);

  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int Partition,
                           int64_t Offset);

private:
  bool Real = true;
  std::unique_ptr<ConsumerInterface> KafkaConsumer;
  std::unique_ptr<Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger;
  UserArgumentStruct UserArguments;
  const std::string SchemaPath;
  void printKafkaMessage(KafkaMessageMetadataStruct &MessageData,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);
  bool verifyOffset(int64_t Offset, const std::string &TopicName);
  void verifyNLast(int64_t NLast, const std::string &TopicName,
                   int16_t Partition);
  void printMessageMetadata(KafkaMessageMetadataStruct &MessageData,
                            const std::string &FileIdentifier);
  void printEntireTopic(const std::string &TopicName,
                        bool TerminateAtEndOfTopic = false);
  void checkIfTopicEmpty(const std::string &TopicName);
  std::string timestampToReadable(const int64_t &Timestamp);
  bool consumeSingleMessage(int &EOFPartitionCounter,
                            FlatbuffersTranslator &FlatBuffers);
};
