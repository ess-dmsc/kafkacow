//
// Created by michal on 15/08/18.
//
#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <CLI/CLI.hpp>
#include "ConnectKafka.h"
#ifndef KAFKACOW_CONSUMERPREPARE_H
#define KAFKACOW_CONSUMERPREPARE_H

class ConsumerPrepare{
private:
    std::shared_ptr<RdKafka::KafkaConsumer> Consumer;

public:
    ConsumerPrepare(std::string Broker, std::string ErrStr);
    std::shared_ptr<RdKafka::KafkaConsumer> GetConsumer();
};




#endif //KAFKACOW_CONSUMERPREPARE_H

