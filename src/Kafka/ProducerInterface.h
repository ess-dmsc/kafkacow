#pragma once

#include "Message.h"
#include <librdkafka/rdkafkacpp.h>

namespace Kafka {

class ProducerInterface {
public:
  virtual ~ProducerInterface() = default;

  virtual void produce(Kafka::Message Message) = 0;

private:
};
} // namespace Kafka
