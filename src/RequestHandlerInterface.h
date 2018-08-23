#pragma once

#include "../src/ConnectKafka.h"
#include <librdkafka/rdkafkacpp.h>

class RequestHandlerInterface {
protected:
  virtual int init() = 0;
  virtual std::string subscribeConsumeAtOffset(std::string TopicName,
                                               int64_t Offset) = 0;
  virtual std::string
  subscribeConsumeNLastMessages(std::string TopicName,
                                int64_t NumberOfMessages) = 0;
};
