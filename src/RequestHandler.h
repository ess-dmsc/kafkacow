#pragma once

#include "CustomExceptions.h"
#include "FlatbuffersTranslator.h"
#include "Kafka/ConsumerFactory.h"
#include "Kafka/ConsumerInterface.h"
#include "Kafka/ProducerFactory.h"
#include "Kafka/ProducerInterface.h"
#include "UserArgumentsStruct.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class RequestHandler {
public:
  explicit RequestHandler(UserArgumentStruct &UserArguments,
                          std::string FullSchemaPath, bool Real = true)
      : Logger(spdlog::get("LOG")), UserArguments(UserArguments),
        SchemaPath(std::move(FullSchemaPath)) {
    bool ConsumerModeChosen = UserArguments.ConsumerMode &&
                              !UserArguments.MetadataMode &&
                              !UserArguments.ProducerMode;
    bool MetadataModeChosen = !UserArguments.ConsumerMode &&
                              UserArguments.MetadataMode &&
                              !UserArguments.ProducerMode;
    bool ProducerModeChosen = UserArguments.ProducerMode &&
                              !UserArguments.ConsumerMode &&
                              !UserArguments.MetadataMode;

    if (ConsumerModeChosen || MetadataModeChosen) {
      KafkaConsumer = Kafka::createConsumer(UserArguments.Broker, Real);
    } else if (ProducerModeChosen) {
      KafkaProducer = Kafka::createProducer(UserArguments.Broker,
                                            UserArguments.TopicName, Real);
    } else {
      throw ArgumentException("Program can run in one and only one mode: "
                              "--consumer, --metadata or --producer");
    }
  }

  void checkAndRun();

  void checkConsumerModeArguments(bool TerminateAtEndOfTopic = false);

  void checkMetadataModeArguments();

  void runProducer();

  void showTopicPartitionOffsets();

  void subscribeAndConsume(const std::string &TopicName, int64_t Offset,
                           bool TerminateAtEndOfTopic = false);
  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int Partition,
                           bool TerminateAtEndOfTopic);

  void subscribeAndConsume(const std::string &TopicName,
                           int64_t NumberOfMessages, int64_t Offset);

private:
  std::unique_ptr<Kafka::ConsumerInterface> KafkaConsumer;
  std::unique_ptr<Kafka::ProducerInterface> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger;
  UserArgumentStruct UserArguments;
  const std::string SchemaPath;
  void printKafkaMessage(Kafka::MessageMetadataStruct &MessageData,
                         int &EOFPartitionCounter,
                         FlatbuffersTranslator &FlatBuffers);
  bool verifyOffset(int64_t Offset, const std::string &TopicName);
  void verifyNLast(int64_t NLast, const std::string &TopicName,
                   int16_t Partition);
  void printEntireTopic(const std::string &TopicName,
                        bool TerminateAtEndOfTopic = false);
  void checkIfTopicEmpty(const std::string &TopicName);
  bool consumeSingleMessage(int &EOFPartitionCounter,
                            FlatbuffersTranslator &FlatBuffers);
  void consumeAllSubscribed(const std::string &Topic,
                            bool TerminateAtEndOfTopic);
  void consumeNSubscribed(const std::string &Topic, int64_t NumberOfMessages);
  int64_t getOffsetForDate(const std::string &Date, const std::string &Topic);
};
