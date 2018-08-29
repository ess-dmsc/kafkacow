#pragma once

#include "../src/ConnectKafka.h"
#include "UserArgumentsStruct.h"
#include <librdkafka/rdkafkacpp.h>

class RequestHandlerInterface {
protected:
  virtual void checkAndRun(UserArgumentStruct UserArguments) = 0;

  virtual void checkConsumerModeArguments(UserArgumentStruct UserArguments) = 0;
  virtual void checkMetadataModeArguments(UserArgumentStruct UserArguments) = 0;

  virtual void showTopicPartitionOffsets(UserArgumentStruct UserArguments) = 0;

  virtual std::string subscribeConsumeAtOffset(std::string TopicName,
                                               int64_t Offset) = 0;
  virtual std::string
  subscribeConsumeNLastMessages(std::string TopicName,
                                int64_t NumberOfMessages) = 0;
};
