#include "Producer.h"
#include "KafkaConfig.h"
#include <fstream>
#include <memory>
#include <sstream>

Producer::Producer(std::string Broker) {
  std::string ErrStr;
  this->KafkaProducer =
      std::shared_ptr<RdKafka::Producer>(RdKafka::Producer::create(
          createGlobalConfiguration(Broker).get(), ErrStr));
  if (!ErrStr.empty()) {
    ErrStr.append(
        "Error creating KafkaProducer in KafkaW::Producer::Producer.");
    Logger->error(ErrStr);
  }
}

void Producer::produceMessage(KafkaW::Message &Message,
                              std::shared_ptr<RdKafka::Topic> Topic) {
  RdKafka::ErrorCode resp;
  do {
    resp =
        KafkaProducer->produce(Topic.get(), RdKafka::Topic::PARTITION_UA,
                               RdKafka::Producer::RK_MSG_COPY, Message.data(),
                               Message.size(), nullptr, nullptr);

    if (resp != RdKafka::ERR_NO_ERROR) {
      if (resp != RdKafka::ERR__QUEUE_FULL) {
        Logger->error("Produce failed: {}\n"
                      "Message size was: {}",
                      RdKafka::err2str(resp), Message.size());
      }
      // This blocking poll call should give Kafka some time for the problem to
      // be resolved
      // for example for messages to leave the queue if it is full
      KafkaProducer->poll(1000);
    } else {
      KafkaProducer->poll(0);
    }
  } while (resp == RdKafka::ERR__QUEUE_FULL);
}

std::shared_ptr<RdKafka::Topic>
Producer::createTopicHandle(const std::string &topicPrefix,
                            const std::string &topicSuffix,
                            std::shared_ptr<RdKafka::Conf> topicConfig) {
  std::string topic_str = topicPrefix + topicSuffix;
  std::string error_str;
  auto topic_ptr = std::shared_ptr<RdKafka::Topic>(RdKafka::Topic::create(
      KafkaProducer.get(), topic_str, topicConfig.get(), error_str));
  if (topic_ptr == nullptr) {
    Logger->error("Failed to create topic: {}", error_str);
    throw std::runtime_error("Failed to create topic");
  }
  return topic_ptr;
}

void Producer::produce(KafkaW::Message Message) {
  auto TopicConfiguration = std::shared_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
  auto TopicHandle = createTopicHandle("TopicA", "TopicB", TopicConfiguration);
  produceMessage(Message, TopicHandle);
}

std::string Producer::loadFromFile(const std::string *Path) {
  std::ifstream File(*Path);
  std::stringstream buffer;
  buffer << File.rdbuf();
  return buffer.str();
}
