#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <spdlog/logger.h>

class Producer {
public:
  Producer(std::string Broker);

private:
  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger;
};
