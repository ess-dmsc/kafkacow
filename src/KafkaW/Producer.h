#pragma once

#include "ProducerInterface.h"
#include <librdkafka/rdkafkacpp.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class Producer : public ProducerInterface {
public:
  Producer(std::string Broker);
  ~Producer() {
    if (KafkaProducer) {
      KafkaProducer->flush(500);
      RdKafka::wait_destroyed(5000);
    }
  }
  void produce(KafkaW::Message Message) override;

private:
  std::shared_ptr<RdKafka::Topic>
  createTopicHandle(const std::string &topicPrefix,
                    const std::string &topicSuffix,
                    std::shared_ptr<RdKafka::Conf> topicConfig);
  void produceMessage(KafkaW::Message &Message,
                      std::shared_ptr<RdKafka::Topic> Topic);

  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
};
