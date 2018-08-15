//
// Created by michal on 15/08/18.
//
#include <iostream>
#include <librdkafka/rdkafkacpp.h>
#include <CLI/CLI.hpp>
#ifndef KAFKACOW_CONNECTKAFKA_H
#define KAFKACOW_CONNECTKAFKA_H

std::unique_ptr<RdKafka::Metadata> queryMetadata(std::shared_ptr<RdKafka::KafkaConsumer> Consumer);

std::unique_ptr<RdKafka::Conf> createGlobalConfiguration(const std::string &BrokerAddr);
#endif //KAFKACOW_CONNECTKAFKA_H
