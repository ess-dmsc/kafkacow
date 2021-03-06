#pragma once

#include "ProducerInterface.h"
#include <librdkafka/rdkafkacpp.h>
#include <map>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Kafka {

class Producer : public ProducerInterface {
public:
  Producer(const std::string &Broker, std::string Topic,
           const std::map<std::string, std::string> &KafkaConfiguration);

  ~Producer() override {
    if (KafkaProducer) {
      KafkaProducer->flush(500);
      RdKafka::wait_destroyed(5000);
    }
  }

  /// Sends message to Kafka.
  /// \param Message Serialized message to be sent.
  void produce(Kafka::Message Message) override;

private:
  std::shared_ptr<RdKafka::Topic>
  createTopicHandle(std::shared_ptr<RdKafka::Conf> topicConfig);

  void produceMessage(Kafka::Message &Message,
                      std::shared_ptr<RdKafka::Topic> Topic);

  std::string TopicToProduce;
  std::shared_ptr<RdKafka::Producer> KafkaProducer;
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
};
} // namespace Kafka
