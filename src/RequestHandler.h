#pragma once

#include "ConnectKafka.h"
#include "RequestHandlerParentClass.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class RequestHandler : public RequestHandlerParentClass {
private:
  std::unique_ptr<ConnectKafkaParentClass> KafkaConnection;

public:
  explicit RequestHandler(
      std::unique_ptr<ConnectKafkaParentClass> KafkaConnection)
      : KafkaConnection(std::move(KafkaConnection)) {}

  virtual int Init() override;
  virtual std::string SubscribeConsumeAtOffset(std::string TopicName,
                                               int64_t Offset) override;
  virtual std::string
  SubscribeConsumeNLastMessages(std::string TopicName,
                                int64_t NumberOfMessages) override;
};
