#pragma once

#include "ArgumentsException.h"
#include "ConnectKafka.h"
#include "RequestHandlerInterface.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class RequestHandler : public RequestHandlerInterface {
private:
  std::unique_ptr<ConnectKafkaInterface> KafkaConnection;

public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaInterface> KafkaConnection)
      : KafkaConnection(std::move(KafkaConnection)) {}

  int init(UserArgumentStruct UserArguments) override;

  void checkAndRun(UserArgumentStruct UserArguments) override;

  void checkConsumerModeArguments(UserArgumentStruct UserArguments) override;

  void checkMetadataModeArguments(UserArgumentStruct UserArguments) override;

  void showTopicPartitionOffsets(UserArgumentStruct UserArguments) override;

  std::string subscribeConsumeAtOffset(std::string TopicName,
                                       int64_t Offset) override;
  std::string subscribeConsumeNLastMessages(std::string TopicName,
                                            int64_t NumberOfMessages) override;
};
