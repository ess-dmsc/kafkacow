#include "Consumer.h"
#include "../CustomExceptions.h"
#include "KafkaConfig.h"
#include "MessageMetadataStruct.h"
#include <date/date.h>
#include <iomanip>

namespace Kafka {

/// Gets Metadata object from Kafka.
///
/// \return unique_ptr to RdKafka::Metadata
std::unique_ptr<RdKafka::Metadata> Consumer::queryMetadata() {
  RdKafka::Metadata *metadataRawPtr(nullptr);
  RdKafka::ErrorCode ErrorCode =
      KafkaConsumer->metadata(true, nullptr, &metadataRawPtr, 1000);
  if (ErrorCode == RdKafka::ERR__TRANSPORT) {
    throw std::runtime_error("Broker does not exist!");
  } else if (ErrorCode != RdKafka::ERR_NO_ERROR) {
    throw std::runtime_error(fmt::format(
        "Error while retrieving metadata. RdKafka errorcode: {}", ErrorCode));
  }
  std::unique_ptr<RdKafka::Metadata> metadata(metadataRawPtr);
  if (metadata == nullptr) {
    throw std::runtime_error("Error while retrieving metadata.");
  }
  return metadata;
}

Consumer::Consumer(std::string Broker) : Logger(spdlog::get("LOG")) {
  std::string ErrStr;
  this->KafkaConsumer =
      std::shared_ptr<RdKafka::KafkaConsumer>(RdKafka::KafkaConsumer::create(
          createGlobalConfiguration(Broker).get(), ErrStr));
  if (!ErrStr.empty()) {
    ErrStr.append("Error creating KafkaConsumer in Consumer::Consumer.");
    Logger->error(ErrStr);
  }
  this->MetadataPointer = this->queryMetadata();
}

/// Returns a list of topics stored by the broker.
///
/// \return single string containing all topics.
std::string Consumer::getAllTopics() {
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
MessageMetadataStruct Consumer::consume() {
  using RdKafka::Message;
  MessageMetadataStruct DataToReturn;

  auto KafkaMsg = std::unique_ptr<Message>(KafkaConsumer->consume(1000));
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
      if (KafkaMsg->key_len() != 0) {
        DataToReturn.Key = *KafkaMsg->key();
        DataToReturn.KeyPresent = true;
      }
    } else {
      // If RdKafka indicates no error then we should always get a
      // non-zero length message
      throw std::runtime_error("KafkaTopicSubscriber::consumeMessage() - Kafka "
                               "indicated no error but a zero-length payload "
                               "was received");
    }
    break;
  case RdKafka::ERR__TIMED_OUT:
    throw TimeoutException(
        fmt::format("KafkaTopicSubscriber::consumeMessage() - {}",
                    RdKafka::err2str(KafkaMsg->err())));
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
  return DataToReturn;
}

/// Queries Kafka for Topic's metadata and returns partition numbers.
///
/// \param Topic
/// \return vector<int32_t> of partition IDs
std::vector<int32_t>
Consumer::getTopicPartitionNumbers(const std::string &Topic) {
  auto Metadata = queryMetadata();
  auto Topics = Metadata->topics();
  auto Iterator = std::find_if(Topics->cbegin(), Topics->cend(),
                               [Topic](const RdKafka::TopicMetadata *tpc) {
                                 return tpc->topic() == Topic;
                               });
  auto MatchedTopic = *Iterator;
  if (MatchedTopic == *Topics->cend())
    throw ArgumentException(fmt::format("No such topic: {}", Topic));

  std::vector<int32_t> TopicPartitionNumbers;
  auto PartitionMetadata = MatchedTopic->partitions();
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
Consumer::getTopicsHighAndLowOffsets(const std::string &Topic) {
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
OffsetsStruct Consumer::getPartitionHighAndLowOffsets(const std::string &Topic,
                                                      int32_t PartitionID) {
  int64_t Low, High;
  KafkaConsumer->query_watermark_offsets(Topic, PartitionID, &Low, &High, 100);
  OffsetsStruct OffsetsToSave{Low, High, PartitionID};
  return OffsetsToSave;
}

/// Returns a number of partitions that specified Topic has.
///
/// \param Topic
/// \return
int Consumer::getNumberOfTopicPartitions(std::string Topic) {
  return getTopicPartitionNumbers(Topic).size();
}

/// Subscribes to partitions of a specified Topic at given Offset.
///
/// \param Offset
/// \param Topic
void Consumer::subscribeAtOffset(int64_t Offset, std::string Topic) {
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsets;
  for (auto i = 0; i < getNumberOfTopicPartitions(Topic); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(Topic, i);

    TopicPartition->set_offset(Offset);
    TopicPartitionsWithOffsets.push_back(TopicPartition);
  }
  KafkaConsumer->assign(TopicPartitionsWithOffsets);
  std::for_each(TopicPartitionsWithOffsets.cbegin(),
                TopicPartitionsWithOffsets.cend(),
                [](RdKafka::TopicPartition *Partition) { delete Partition; });
}

/// Subscribes to a specified Partition of a Topic to get last NMessages.
///
/// \param NMessages
/// \param Topic
/// \param Partition
void Consumer::subscribeToLastNMessages(int64_t NMessages,
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
  KafkaConsumer->assign(TopicPartitionsWithOffsetsSet);
  std::for_each(TopicPartitionsWithOffsetsSet.cbegin(),
                TopicPartitionsWithOffsetsSet.cend(),
                [](RdKafka::TopicPartition *Partition) { delete Partition; });
}

/// Displays brokers, topics, partitions and their details.
///
/// \return single string containing metadata.
std::string Consumer::showAllMetadata() {
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

long Consumer::isoDateToTimestamp(const std::string &Date) {
  std::istringstream ss(Date);
  std::chrono::system_clock::time_point tp;
  ss >> date::parse("%Y-%m-%dT%H:%M:%S", tp);
  if (tp.time_since_epoch().count() == 0)
    throw ArgumentException("Date not valid. Please use ISO8601 format, "
                            "e.g.[2019-07-05T08:18:14.366].");
  return tp.time_since_epoch().count();
}

int64_t Consumer::getOffsetForDate(const std::string &Date,
                                   const std::string &Topic) {
  int64_t Timestamp = isoDateToTimestamp(Date) / 1000000;
  std::vector<RdKafka::TopicPartition *> TopicPartitionsWithOffsetsSet;
  for (int i = 0; i < getNumberOfTopicPartitions(Topic); i++) {
    auto TopicPartition = RdKafka::TopicPartition::create(Topic, i);
    TopicPartition->set_offset(Timestamp);
    TopicPartitionsWithOffsetsSet.push_back(TopicPartition);
  }
  KafkaConsumer->offsetsForTimes(TopicPartitionsWithOffsetsSet, 1000);
  return std::min_element(TopicPartitionsWithOffsetsSet.begin(),
                          TopicPartitionsWithOffsetsSet.end(),
                          [](RdKafka::TopicPartition *First,
                             RdKafka::TopicPartition *Second) {
                            return First->offset() > Second->offset();
                          })
      .
      operator*()
      ->offset();
}
}
