#include "ConnectKafka.h"
#include "KafkaMessageMetadataStruct.h"
#include <iomanip>

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

/// Gets Metadata object from Kafka.
///
/// \return unique_ptr to RdKafka::Metadata
std::unique_ptr<RdKafka::Metadata> ConnectKafka::queryMetadata() {
  RdKafka::Metadata *metadataRawPtr(nullptr);
  Consumer->metadata(true, nullptr, &metadataRawPtr, 1000);
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
  Logger = spdlog::get("LOG");
}

/// Returns a list of topics stored by the broker.
///
/// \return single string containing all topics.
std::string ConnectKafka::getAllTopics() {
  auto Topics = MetadataPointer->topics();
  std::string ListOfTopics;
  for (const auto &TopicName : *Topics) {
    ListOfTopics.append(TopicName->topic());
    ListOfTopics += '\n';
  }
  return ListOfTopics;
}

/// Consumes Kafka messages starting from specified offset.
///
/// \return struct containg serialized message and its metadata.
KafkaMessageMetadataStruct ConnectKafka::consumeFromOffset() {
  using RdKafka::Message;
  KafkaMessageMetadataStruct DataToReturn;

  auto KafkaMsg = std::unique_ptr<Message>(Consumer->consume(1000));
  switch (KafkaMsg->err()) {
  case RdKafka::ERR_NO_ERROR:
    // Real message
    if (KafkaMsg->len() > 0) {
      std::string Payload(static_cast<const char *>(KafkaMsg->payload()),
                          static_cast<int>(KafkaMsg->len()));
      DataToReturn.Payload = Payload;
      DataToReturn.Partition = KafkaMsg->partition();
      DataToReturn.Offset = KafkaMsg->offset();
      DataToReturn.Timestamp = KafkaMsg->timestamp().timestamp;

    } else {
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
    DataToReturn.PartitionEOF = true;
    // Not errors as the broker might come back or more data might be pushed
    break;

  default:
    /* All other errors */
    std::ostringstream os;
    os << "KafkaTopicSubscriber::consumeMessage() - "
       << RdKafka::err2str(KafkaMsg->err());
    throw std::runtime_error(os.str());
  }
  return DataToReturn;
}

/// Queries Kafka for Topic's metadata and returns partition numbers.
///
/// \param Topic
/// \return vector<int32_t> of partition IDs
std::vector<int32_t> ConnectKafka::getTopicPartitionNumbers(std::string Topic) {
  auto Metadata = queryMetadata();
  auto Topics = Metadata->topics();
  auto Iterator = std::find_if(Topics->cbegin(), Topics->cend(),
                               [Topic](const RdKafka::TopicMetadata *tpc) {
                                 return tpc->topic() == Topic;
                               });
  auto matchedTopic = *Iterator;
  std::vector<int32_t> TopicPartitionNumbers;
  auto PartitionMetadata = matchedTopic->partitions();
  // save needed partition metadata here
  for (auto &Partition : *PartitionMetadata) {
    TopicPartitionNumbers.push_back(Partition->id());
  }
  sort(TopicPartitionNumbers.begin(), TopicPartitionNumbers.end());
  return TopicPartitionNumbers;
}

/// Returns a vector of structs containing offsets of partitions for specified
/// Topic.
///
/// \param Topic
/// \return
std::vector<OffsetsStruct>
ConnectKafka::getTopicsHighAndLowOffsets(std::string Topic) {
  auto TopicPartitions = getTopicPartitionNumbers(Topic);
  std::vector<OffsetsStruct> HighAndLowOffsets;
  for (auto &PartitionID : TopicPartitions) {
    OffsetsStruct OffsetsToSave =
        getPartitionHighAndLowOffsets(Topic, PartitionID);
    HighAndLowOffsets.push_back(OffsetsToSave);
  }
  return HighAndLowOffsets;
}

/// Queries Kafka for offsets of a single PartitionID for a specified Topic
///
/// \param Topic
/// \param PartitionID
/// \return OffsetsStruct containing PartitionID and High- and LowOffset.
OffsetsStruct
ConnectKafka::getPartitionHighAndLowOffsets(const std::string &Topic,
                                            int32_t PartitionID) {
  int64_t Low, High;
  Consumer->query_watermark_offsets(Topic, PartitionID, &Low, &High, 100);
  OffsetsStruct OffsetsToSave;
  OffsetsToSave.HighOffset = High;
  OffsetsToSave.LowOffset = Low;
  OffsetsToSave.PartitionId = PartitionID;
  return OffsetsToSave;
}

/// Returns a number of partitions that specified Topic has.
///
/// \param Topic
/// \return
int ConnectKafka::getNumberOfTopicPartitions(std::string Topic) {
  return getTopicPartitionNumbers(Topic).size();
}

/// Subscribes to partitions of a specified Topic at given Offset.
///
/// \param Offset
/// \param Topic
void ConnectKafka::subscribeAtOffset(int64_t Offset, std::string Topic) {
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsets;
  for (auto i = 0; i < getNumberOfTopicPartitions(Topic); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(Topic, i);

    TopicPartition->set_offset(Offset);
    TopicPartitionsWithOffsets.push_back(TopicPartition);
  }
  Consumer->assign(TopicPartitionsWithOffsets);
  std::for_each(TopicPartitionsWithOffsets.cbegin(),
                TopicPartitionsWithOffsets.cend(),
                [](RdKafka::TopicPartition *Partition) { delete Partition; });
}

KafkaMessageMetadataStruct ConnectKafka::consumeLastNMessages() {
  return consumeFromOffset();
}

/// Subscribes to a specified Partition of a Topic to get last NMessages.
///
/// \param NMessages
/// \param Topic
/// \param Partition
void ConnectKafka::subscribeToLastNMessages(int64_t NMessages,
                                            const std::string &Topic,
                                            int Partition) {
  std::vector<OffsetsStruct> HighAndLowOffsets =
      getTopicsHighAndLowOffsets(Topic);

  // get highest offset of all partitions
  int64_t HighestOffest = 0;
  for (auto it : HighAndLowOffsets) {
    if (it.HighOffset > HighestOffest)
      HighestOffest = it.HighOffset;
  }
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsetsSet;
  for (auto i = 0; i < getNumberOfTopicPartitions(Topic); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(Topic, i);

    // subscribe to the chosen partition
    if (i == Partition) {
      TopicPartition->set_offset(HighAndLowOffsets[Partition].HighOffset -
                                 NMessages + 0);
    } else {
      TopicPartition->set_offset(HighestOffest + 1000);
    }
    TopicPartitionsWithOffsetsSet.push_back(TopicPartition);
  }
  Consumer->assign(TopicPartitionsWithOffsetsSet);
  std::for_each(TopicPartitionsWithOffsetsSet.cbegin(),
                TopicPartitionsWithOffsetsSet.cend(),
                [](RdKafka::TopicPartition *Partition) { delete Partition; });
}

/// Displays brokers, topics, partitions and their details.
///
/// \return single string containing metadata.
std::string ConnectKafka::showAllMetadata() {
  using std::setw;
  std::stringstream SS;
  SS << MetadataPointer->brokers()->size() << " brokers:\n";
  for (auto Broker : *MetadataPointer->brokers())
    SS << "   broker " << Broker->id() << " at " << Broker->host() << ":"
       << Broker->port() << "\n";
  SS << "\n";
  SS << MetadataPointer->topics()->size() << " topics:\n";
  for (auto Topic : *MetadataPointer->topics()) {
    SS << "   \"" << Topic->topic() << "\" with " << Topic->partitions()->size()
       << " partitions:\n";
    for (auto Partition : *Topic->partitions()) {
      OffsetsStruct PartitionOffsets =
          getPartitionHighAndLowOffsets(Topic->topic(), Partition->id());
      std::stringstream Replicas;
      std::copy(Partition->replicas()->begin(), Partition->replicas()->end(),
                std::ostream_iterator<int32_t>(Replicas, ", "));
      std::stringstream ISRSs;
      std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
                std::ostream_iterator<int32_t>(ISRSs, ", "));
      SS << "        partition " << setw(3) << Partition->id()
         << "  |  Low offset: " << setw(6) << PartitionOffsets.LowOffset
         << "  |  High Offset: " << setw(6) << PartitionOffsets.HighOffset
         << "  |  leader: " << setw(3) << Partition->leader()
         << "  |  replicas: " << Replicas.str() << "|  isrs: " << ISRSs.str()
         << "\n";
    }
    SS << "\n";
  }
  std::string Metadata = SS.str();
  return Metadata;
}
