#pragma once

#include "ProducerInterface.h"
#include <librdkafka/rdkafkacpp.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class Producer : public ProducerInterface {
public:
  Producer(std::string Broker, std::string Topic);
  ~Producer() {
    if (KafkaProducer) {
      KafkaProducer->flush(500);
      RdKafka::wait_destroyed(5000);
    }
  }
  void produce(KafkaW::Message Message) override;

private:
  std::shared_ptr<RdKafka::Topic>
  createTopicHandle(std::shared_ptr<RdKafka::Conf> topicConfig);
  void produceMessage(KafkaW::Message &Message,
                      std::shared_ptr<RdKafka::Topic> Topic);

  std::string TopicToProduce;
  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
};
