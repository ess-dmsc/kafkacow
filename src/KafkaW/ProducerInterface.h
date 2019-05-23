#pragma once

#include "Message.h"
#include <librdkafka/rdkafkacpp.h>

class ProducerInterface {
public:
  virtual ~ProducerInterface() = default;

  virtual void produce(KafkaW::Message &Message) = 0;
};
