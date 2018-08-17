#include "ConnectKafka.h"

namespace {
std::unique_ptr<RdKafka::Conf>
createGlobalConfiguration(const std::string &BrokerAddr) {
  auto conf = std::unique_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  std::string ErrStr;
  conf->set("metadata.broker.list", BrokerAddr, ErrStr);
  conf->set("session.timeout.ms", "10000", ErrStr);
  conf->set("group.id", "mantid", ErrStr);
  conf->set("message.max.bytes", "10000000", ErrStr);
  conf->set("fetch.message.max.bytes", "10000000", ErrStr);
  conf->set("replica.fetch.max.bytes", "10000000", ErrStr);
  conf->set("enable.auto.commit", "false", ErrStr);
  conf->set("enable.auto.offset.store", "false", ErrStr);
  conf->set("offset.store.method", "none", ErrStr);
  conf->set("api.version.request", "true", ErrStr);
  return conf;
}
}

std::unique_ptr<RdKafka::Metadata> ConnectKafka::queryMetadata() {
  RdKafka::Metadata *metadataRawPtr(nullptr);
  // API requires address of a pointer to the struct but compiler won't allow
  // &metadata.get() as it is an rvalue

  Consumer->metadata(true, nullptr, &metadataRawPtr, 1000);
  // Capture the pointer in an owning struct to take care of deletion
  std::unique_ptr<RdKafka::Metadata> metadata(metadataRawPtr);
  if (!metadata) {
    throw std::runtime_error("Failed to query metadata from broker");
  }
  return metadata;
}

ConnectKafka::ConnectKafka(std::string Broker, std::string ErrStr) {
  this->Consumer =
      std::shared_ptr<RdKafka::KafkaConsumer>(RdKafka::KafkaConsumer::create(
          createGlobalConfiguration(Broker).get(), ErrStr));
  this->MetadataPointer = this->queryMetadata();
}

std::string ConnectKafka::GetAllTopics() {
  auto Topics = MetadataPointer->topics();
  std::string ListOfTopics = "";
  for (const auto &TopicName : *Topics) {
    ListOfTopics.append(TopicName->topic());
    ListOfTopics += '\n';
  }
  return ListOfTopics;
}

std::string
ConnectKafka::SubscribeToTopic(const std::vector<std::string> &Topic) {

  this->Consumer->subscribe(Topic);
}

bool ConnectKafka::CheckIfTopicExists(std::string Topic) {
  std::string AllTopics = GetAllTopics();
  return AllTopics.find(Topic) != std::string::npos;
}
