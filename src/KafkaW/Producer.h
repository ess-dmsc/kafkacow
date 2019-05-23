#pragma once

#include "ProducerInterface.h"
#include <librdkafka/rdkafkacpp.h>
#include <spdlog/logger.h>

class Producer : public ProducerInterface {
public:
  Producer(std::string Broker);
  void produce() override;

private:
  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger;
};
