#pragma once

#include "CustomExceptions.h"
#include "FlatbuffersTranslator.h"
#include "KafkaW/ConsumerFactory.h"
#include "KafkaW/ConsumerInterface.h"
#include "KafkaW/ProducerFactory.h"
#include "KafkaW/ProducerInterface.h"
#include "UserArgumentsStruct.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler {
public:
  explicit RequestHandler(UserArgumentStruct &UserArguments,
                          std::string FullSchemaPath, bool Real = true)
      : Logger(spdlog::get("LOG")), UserArguments(UserArguments),
        SchemaPath(std::move(FullSchemaPath)) {
    // check input if ConsumerMode chosen
    if (UserArguments.ConsumerMode && !UserArguments.MetadataMode &&
        !UserArguments.ProducerMode) {
      KafkaConsumer = KafkaW::createConsumer(UserArguments.Broker, Real);
    }
    // check input if MetadataMode chosen
    else if (!UserArguments.ConsumerMode && UserArguments.MetadataMode &&
             !UserArguments.ProducerMode) {
      KafkaConsumer = KafkaW::createConsumer(UserArguments.Broker, Real);
    } else if (UserArguments.ProducerMode && !UserArguments.ConsumerMode &&
               !UserArguments.MetadataMode) {
      KafkaProducer = KafkaW::createProducer(UserArguments.Broker, Real);
    }
    // no MetadataMode or ConsumerMode chosen
    else
      throw ArgumentException("Program can run in one and only one mode: "
                              "--consumer, --metadata or --producer");
  }

  void checkAndRun();

  void checkConsumerModeArguments(bool TerminateAtEndOfTopic = false);

  void checkMetadataModeArguments();

  void runProducer();

  void showTopicPartitionOffsets();

  void subscribeAndConsume(const std::string &TopicName, int64_t Offset,
                           bool TerminateAtEndOfTopic = false);
  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int Partition);

  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int Partition,
                           int64_t Offset);

private:
  std::unique_ptr<ConsumerInterface> KafkaConsumer;
  std::unique_ptr<ProducerInterface> KafkaProducer;
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
