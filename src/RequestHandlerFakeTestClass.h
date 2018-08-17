#pragma once

#include "ConnectKafka.h"
#include "RequestHandler.h"
#include "RequestHandlerParentClass.h"
#include <CLI/CLI.hpp>

class RequestHandlerFakeTestClass : public RequestHandlerParentClass {
private:
  std::unique_ptr<ConnectKafka> KafkaConnection;

public:
  explicit RequestHandlerFakeTestClass(
      std::unique_ptr<ConnectKafka> KafkaConnectionArgument)
      : KafkaConnection(std::move(KafkaConnection)) {}
  virtual void PrintToScreen(std::string ToPrint);
  virtual std::string GetAllTopics() override;
  virtual bool CheckIfTopicExists(std::string Topic) override;
  virtual void Consume(std::string Topic) override;
  virtual void SubscribeToTopic(const std::vector<std::string> &Topic) override;
};
