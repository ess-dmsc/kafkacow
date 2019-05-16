#pragma once

#include "KafkaConfig.h"
#include <librdkafka/rdkafkacpp.h>

class Producer {
  Producer(std::string Broker);

private:
  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger;
};
