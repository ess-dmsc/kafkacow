#pragma once

#include "../src/ConnectKafka.h"
#include <librdkafka/rdkafkacpp.h>

class RequestHandlerParentClass {
protected:
  virtual void PrintToScreen(std::string ToPrint) = 0;
  virtual std::string GetAllTopics() = 0;
  virtual bool CheckIfTopicExists(std::string Topic) = 0;
  virtual void Consume(std::string Topic) = 0;
  virtual void SubscribeToTopic(const std::vector<std::string> &Topic) = 0;
  virtual void GetHighLowOffsets(std::string Topic)=0;
};