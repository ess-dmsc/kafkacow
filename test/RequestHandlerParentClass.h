#pragma once

#include "../src/ConnectKafka.h"
#include <librdkafka/rdkafkacpp.h>

class RequestHandlerParentClass {
protected:
  virtual void PrintToScreen(std::string ToPrint) = 0;
  virtual std::string GetAllTopics() = 0;
};