#include "ConnectKafka.h"
#include "TopicMetadataStruct.h"

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
  conf->set("auto.offset.reset", "largest", ErrStr);
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

void ConnectKafka::SubscribeToTopic(const std::vector<std::string> &Topic) {
  this->Consumer->subscribe(Topic);
}

bool ConnectKafka::CheckIfTopicExists(std::string Topic) {
  std::string AllTopics = GetAllTopics();
  return AllTopics.find(Topic) != std::string::npos;
}

std::string ConnectKafka::Consume(std::string Topic) {
  using RdKafka::Message;
  std::string *payload;
  std::string topic;

  auto kfMsg = std::unique_ptr<Message>(Consumer->consume(100));
  switch (kfMsg->err()) {
  case RdKafka::ERR_NO_ERROR:
    // Real message
    if (kfMsg->len() > 0) {
      payload->assign(static_cast<const char *>(kfMsg->payload()),
                      static_cast<int>(kfMsg->len()));
      //      offset = kfMsg->offset();
      //      partition = kfMsg->partition();
      topic = kfMsg->topic_name();
      std::cout << *payload << std::endl;
      std::cout << topic << std::endl;
      std::cout << "did i even get here?" << std::endl;
    } else {
      // If RdKafka indicates no error then we should always get a
      // non-zero length message
      throw std::runtime_error("KafkaTopicSubscriber::consumeMessage() - Kafka "
                               "indicated no error but a zero-length payload "
                               "was received");
    }
    break;

  case RdKafka::ERR__TIMED_OUT:
  case RdKafka::ERR__PARTITION_EOF:
    // Not errors as the broker might come back or more data might be pushed
    break;

  default:
    /* All other errors */
    std::ostringstream os;
    os << "KafkaTopicSubscriber::consumeMessage() - "
       << RdKafka::err2str(kfMsg->err());
    throw std::runtime_error(os.str());
  }
}

std::vector<int32_t> ConnectKafka::GetTopicPartitions(std::string Topic) {
  auto TopicMetadata = GetTopicMetadata(Topic);
  return TopicMetadata.Partitions;
}

TopicMetadataStruct ConnectKafka::GetTopicMetadata(std::string TopicName) {
  auto Metadata = queryMetadata();
  auto Topics = Metadata->topics();
  auto iter = std::find_if(Topics->cbegin(), Topics->cend(),
                           [TopicName](const RdKafka::TopicMetadata *tpc) {
                             return tpc->topic() == TopicName;
                           });
  auto matchedTopic = *iter;
  TopicMetadataStruct TopicMetadata;
  TopicMetadata.Name = matchedTopic->topic();
  auto PartitionMetadata = matchedTopic->partitions();

  // save needed partition metadata here
  for (auto &Partition : *PartitionMetadata) {
    TopicMetadata.Partitions.push_back(Partition->id());
  }
  return TopicMetadata;
}

std::unique_ptr<int64_t> ConnectKafka::GetCurrentPartitionOffset(
    const RdKafka::TopicMetadata::PartitionMetadataVector *) {
  return std::unique_ptr<int64_t>();
}
std::vector<OffsetsStruct>
ConnectKafka::GetHighAndLowOffsets(std::string Topic) {
  auto TopicPartitions = GetTopicPartitions(Topic);

  int64_t Low, High;
  int Timeout = 100;
  std::vector<OffsetsStruct> HighAndLowOffsets;

  for (auto &PartitionID : TopicPartitions) {
    Consumer->query_watermark_offsets(Topic, PartitionID, &Low, &High, Timeout);
    OffsetsStruct OffsetsToSave;
    OffsetsToSave.HighOffset = High;
    OffsetsToSave.LowOffset = Low;
    OffsetsToSave.PartitionId = PartitionID;
    HighAndLowOffsets.push_back(OffsetsToSave);
  }
  return HighAndLowOffsets;
}
