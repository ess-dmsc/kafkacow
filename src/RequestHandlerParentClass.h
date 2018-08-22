#pragma once

#include "../src/ConnectKafka.h"
#include <librdkafka/rdkafkacpp.h>

class RequestHandlerParentClass {
protected:
  virtual int Init() = 0;
  virtual std::string SubscribeConsumeAtOffset(std::string TopicName,
                                               int64_t Offset) = 0;
  virtual std::string
  SubscribeConsumeNLastMessages(std::string TopicName,
                                int64_t NumberOfMessages) = 0;
};