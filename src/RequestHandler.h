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

  void PrintToScreen(std::string ToPrint) override;

  virtual std::string GetAllTopics() override;

  virtual bool CheckIfTopicExists(std::string Topic) override;

  virtual void Consume(std::string Topic) override;

  virtual std::vector<OffsetsStruct>
  GetHighLowOffsets(std::string Topic) override;
};
