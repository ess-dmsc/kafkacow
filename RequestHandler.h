//
// Created by michal on 15/08/18.
//
#include <CLI/CLI.hpp>
#include <iostream>
#include <librdkafka/rdkafkacpp.h>

#ifndef KAFKACOW_TOPICHANDLER_H
#define KAFKACOW_TOPICHANDLER_H

class RequestHandler {
private:
  std::shared_ptr<RdKafka::KafkaConsumer> Consumer;
  std::unique_ptr<RdKafka::Metadata> MetadataPointer;

public:
  explicit RequestHandler(std::shared_ptr<RdKafka::KafkaConsumer> Consumer)
      : Consumer(std::move(Consumer)) {}

  std::string PrintAllTopics();
};

#endif // KAFKACOW_TOPICHANDLER_H
