#pragma once

#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <CLI/CLI.hpp>

class ConnectKafka {
    std::shared_ptr<RdKafka::KafkaConsumer> Consumer;

public:
    std::shared_ptr<RdKafka::KafkaConsumer> GetConsumer();
    ConnectKafka(std::string Broker, std::string ErrStr);
  std::unique_ptr<RdKafka::Metadata> queryMetadata();
};
