#include "ConnectKafka.h"
#include "KafkaMessageMetadataStruct.h"
#include <iomanip>

namespace {

void logError(std::array<std::string, 10> ErrStr,
              std::shared_ptr<spdlog::logger> Logger) {
  for (std::string Err : ErrStr) {
    if (!Err.empty()) {
      Err.append(" in createGlobalConfiguration([...])");
      Logger->error(Err);
    }
  }
}

std::unique_ptr<RdKafka::Conf>
createGlobalConfiguration(const std::string &BrokerAddr) {
  auto conf = std::unique_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  std::array<std::string, 10> ErrStr;
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
  conf->set("metadata.broker.list", BrokerAddr, ErrStr[0]);
  conf->set("session.timeout.ms", "10000", ErrStr[1]);
  conf->set("group.id", "mantid", ErrStr[2]);
  conf->set("message.max.bytes", "10000000", ErrStr[3]);
  conf->set("fetch.message.max.bytes", "10000000", ErrStr[4]);
  conf->set("enable.auto.commit", "false", ErrStr[5]);
  conf->set("enable.auto.offset.store", "false", ErrStr[6]);
  conf->set("offset.store.method", "none", ErrStr[7]);
  conf->set("api.version.request", "true", ErrStr[8]);
  conf->set("auto.offset.reset", "largest", ErrStr[9]);
  logError(ErrStr, Logger);
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
  try {
    if (!metadata) {
      throw std::runtime_error("Failed to query metadata from broker");
    }
  } catch (std::runtime_error &E) {
    Logger->error(E.what());
  } catch (std::exception &E) {
    Logger->error(E.what());
  }
  return metadata;
}

ConnectKafka::ConnectKafka(std::string Broker) : Logger(spdlog::get("LOG")) {
  std::string ErrStr;
  this->Consumer =
      std::shared_ptr<RdKafka::KafkaConsumer>(RdKafka::KafkaConsumer::create(
          createGlobalConfiguration(Broker).get(), ErrStr));
  if (!ErrStr.empty())
    Logger->error(ErrStr);
  this->MetadataPointer = this->queryMetadata();
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
  try {
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
        throw std::runtime_error(
            "KafkaTopicSubscriber::consumeMessage() - Kafka "
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
      throw std::runtime_error(
          fmt::format("KafkaTopicSubscriber::consumeMessage() - {}",
                      RdKafka::err2str(KafkaMsg->err())));
    }
  } catch (std::runtime_error &E) {
    Logger->error(E.what());
  } catch (std::exception &E) {
    Logger->error(E.what());
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
  int64_t HighestOffset = 0;
  for (auto it : HighAndLowOffsets) {
    if (it.HighOffset > HighestOffset) {
      HighestOffset = it.HighOffset;
    }
  }
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsetsSet;
  for (auto i = 0; i < getNumberOfTopicPartitions(Topic); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(Topic, i);

    // subscribe to the chosen partition
    if (i == Partition) {
      TopicPartition->set_offset(HighAndLowOffsets[Partition].HighOffset -
                                 NMessages + 0);
    } else {
      TopicPartition->set_offset(HighestOffset + 1000);
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
  for (auto Broker : *MetadataPointer->brokers()) {
    SS << fmt::format("   broker {} at {}:{}\n", Broker->id(), Broker->host(),
                      Broker->port());
  }
  SS << fmt::format("\n{} topics:\n", MetadataPointer->topics()->size());
  for (auto Topic : *MetadataPointer->topics()) {
    SS << fmt::format("   \"{}\" with {} partitions:\n", Topic->topic(),
                      Topic->partitions()->size());
    for (auto Partition : *Topic->partitions()) {
      OffsetsStruct PartitionOffsets =
          getPartitionHighAndLowOffsets(Topic->topic(), Partition->id());
      std::stringstream Replicas;
      std::copy(Partition->replicas()->begin(), Partition->replicas()->end(),
                std::ostream_iterator<int32_t>(Replicas, ", "));
      std::stringstream ISRSs;
      std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
                std::ostream_iterator<int32_t>(ISRSs, ", "));
      SS << fmt::format("        partition {:>3}  |  Low offset: {:>6}  |  "
                        "High offset: {:>6} |  leader: {:>2} |  replicas: {} | "
                        " isrs: {}\n",
                        Partition->id(), PartitionOffsets.LowOffset,
                        PartitionOffsets.HighOffset, Partition->leader(),
                        Replicas.str(), ISRSs.str());
    }
    SS << "\n";
  }
  return SS.str();
}
