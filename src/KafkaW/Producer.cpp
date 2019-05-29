#include "Producer.h"
#include "KafkaConfig.h"
#include <memory>
#include <sstream>

Producer::Producer(std::string Broker, std::string Topic)
    : TopicToProduce(Topic) {
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

/// Sends contents of Message to Topic.
/// \param Message Object containing data to send.
/// \param Topic Target topic.
void Producer::produceMessage(KafkaW::Message &Message,
                              std::shared_ptr<RdKafka::Topic> Topic) {
  RdKafka::ErrorCode ErrorCode;
  do {
    ErrorCode =
        KafkaProducer->produce(Topic.get(), RdKafka::Topic::PARTITION_UA,
                               RdKafka::Producer::RK_MSG_COPY, Message.data(),
                               Message.size(), nullptr, nullptr);

    if (ErrorCode != RdKafka::ERR_NO_ERROR) {
      if (ErrorCode != RdKafka::ERR__QUEUE_FULL) {
        Logger->error("Produce failed: {}\n"
                      "Message size was: {}",
                      RdKafka::err2str(ErrorCode), Message.size());
      }
      // This blocking poll call should give Kafka some time for the problem to
      // be resolved
      // for example for messages to leave the queue if it is full
      KafkaProducer->poll(1000);
    } else {
      KafkaProducer->poll(0);
    }
  } while (ErrorCode == RdKafka::ERR__QUEUE_FULL);
}

/// Creates an RdKafka::Topic handle of target topic.
/// \param topicConfig target topic configuration.
std::shared_ptr<RdKafka::Topic>
Producer::createTopicHandle(std::shared_ptr<RdKafka::Conf> topicConfig) {
  std::string ErrorString;
  auto KafkaTopic = std::shared_ptr<RdKafka::Topic>(RdKafka::Topic::create(
      KafkaProducer.get(), TopicToProduce, topicConfig.get(), ErrorString));
  if (KafkaTopic == nullptr) {
    Logger->error("Failed to create topic: {}", ErrorString);
    throw std::runtime_error("Failed to create topic");
  }
  return KafkaTopic;
}

void Producer::produce(KafkaW::Message Message) {
  auto TopicConfiguration = std::shared_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
  auto TopicHandle = createTopicHandle(TopicConfiguration);
  produceMessage(Message, TopicHandle);
}
