#pragma once

#include "../test/RequestHandlerParentClass.h"
#include "ConnectKafka.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

class RequestHandler : public RequestHandlerParentClass {
private:
  std::unique_ptr<ConnectKafka> KafkaConnection;

public:
  explicit RequestHandler(std::unique_ptr<ConnectKafka> KafkaConnection)
      : KafkaConnection(std::move(KafkaConnection)) {}
  void PrintToScreen(std::string ToPrint) override;
  virtual std::string GetAllTopics() override;
};
