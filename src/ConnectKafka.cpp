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

std::pair<std::string, bool>
ConnectKafka::ConsumeFromOffset(std::string Topic) {
  using RdKafka::Message;
  std::string PayloadToReturn;
  std::string topic;
  bool PartitionEOF = false;

  auto kfMsg = std::unique_ptr<Message>(Consumer->consume(1000));
  switch (kfMsg->err()) {
  case RdKafka::ERR_NO_ERROR:
    // Real message
    if (kfMsg->len() > 0) {
      // std::cout << "did i even get here?" << std::endl;
      std::string Payload(static_cast<const char *>(kfMsg->payload()),
                          static_cast<int>(kfMsg->len()));
      //      offset = kfMsg->offset();
      // partition = kfMsg->partition();
      topic = kfMsg->topic_name();
      // std::cout << Payload << std::endl;
      // std::cout << topic << std::endl;
      PayloadToReturn = Payload;
    } else {
      // std::cout << " here?" << std::endl;
      // If RdKafka indicates no error then we should always get a
      // non-zero length message
      throw std::runtime_error("KafkaTopicSubscriber::consumeMessage() - Kafka "
                               "indicated no error but a zero-length payload "
                               "was received");
    }
    break;

  case RdKafka::ERR__TIMED_OUT:
    break;
  case RdKafka::ERR__PARTITION_EOF:
    PartitionEOF = true;
    // Not errors as the broker might come back or more data might be pushed
    break;

  default:
    /* All other errors */
    std::ostringstream os;
    os << "KafkaTopicSubscriber::consumeMessage() - "
       << RdKafka::err2str(kfMsg->err());
    throw std::runtime_error(os.str());
  }
  return std::make_pair(PayloadToReturn, PartitionEOF);
}

std::vector<int32_t> ConnectKafka::GetTopicPartitionNumbers(std::string Topic) {
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
  auto TopicPartitions = GetTopicPartitionNumbers(Topic);

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

std::vector<RdKafka::TopicPartition *>
ConnectKafka::GetTopicPartitions(std::string TopicName) {
  std::vector<RdKafka::TopicPartition *> Partitions;
  auto Topics = queryMetadata()->topics();
  auto iter = std::find_if(Topics->cbegin(), Topics->cend(),
                           [TopicName](const RdKafka::TopicMetadata *tpc) {
                             return tpc->topic() == TopicName;
                           });
  auto matchedTopic = *iter;
  auto PartitionMetadata = matchedTopic->partitions();
  for (auto &Partition : *PartitionMetadata) {
    auto TopicPartition = RdKafka::TopicPartition::create(
        TopicName, static_cast<int>(Partition->id()));
    Partitions.push_back(TopicPartition);
  }
  return Partitions;
}

int64_t ConnectKafka::GetNumberOfTopicPartitions(std::string TopicName) {
  return GetTopicPartitionNumbers(TopicName).size();
}

void ConnectKafka::SubscribeAtOffset(int64_t Offset, std::string TopicName) {
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsets;
  for (auto i = 0; i < GetNumberOfTopicPartitions(TopicName); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(TopicName, i);

    TopicPartition->set_offset(Offset);
    TopicPartitionsWithOffsets.push_back(TopicPartition);
  }
  Consumer->assign(TopicPartitionsWithOffsets);
  std::for_each(TopicPartitionsWithOffsets.cbegin(),
                TopicPartitionsWithOffsets.cend(),
                [](RdKafka::TopicPartition *partition) { delete partition; });
}

std::pair<std::string, bool>
ConnectKafka::ConsumeLastNMessages(std::string Topic,
                                   int64_t NumberOfMessages) {
  return ConsumeFromOffset(Topic);
}

void ConnectKafka::SubscribeToLastNMessages(int64_t NMessages,
                                            std::string TopicName) {
  std::vector<OffsetsStruct> HighAndLowOffsets =
      GetHighAndLowOffsets(TopicName);
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsets;
  for (auto i = 0; i < GetNumberOfTopicPartitions(TopicName); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(TopicName, i);

    TopicPartition->set_offset(HighAndLowOffsets[i].HighOffset - NMessages);
    TopicPartitionsWithOffsets.push_back(TopicPartition);
  }
  Consumer->assign(TopicPartitionsWithOffsets);
  std::for_each(TopicPartitionsWithOffsets.cbegin(),
                TopicPartitionsWithOffsets.cend(),
                [](RdKafka::TopicPartition *partition) { delete partition; });
}
