#include "Metadata.h"

#include <librdkafka/rdkafkacpp.h>

namespace Metadata {
Cluster::Cluster(std::unique_ptr<RdKafka::Metadata> const &KafkaMetadata) {
  std::vector<Broker> BrokersList(KafkaMetadata->brokers()->size());
  for (auto KafkaBroker : *KafkaMetadata->brokers()) {
    BrokersList.insert(
        BrokersList.cbegin() + static_cast<size_t>(KafkaBroker->id()),
        {KafkaBroker->id(), KafkaBroker->host(), KafkaBroker->port()});
  }
  Brokers = std::move(BrokersList);

  std::vector<Topic> TopicsList(KafkaMetadata->topics()->size());
  for (auto KafkaTopic : *KafkaMetadata->topics()) {
    std::vector<Partition> PartitionsList(KafkaTopic->partitions()->size());
    for (auto Partition : *KafkaTopic->partitions()) {
      PartitionsList.insert(BrokersList.cbegin() +
                                static_cast<size_t>(Partition->id()),
                            getPartitionHighAndLowOffsets(KafkaTopic->topic(), Partition->id()));
    }

    TopicsList.push_back({KafkaTopic->topic(), PartitionsList});


    Metadata::Partition PartitionOffsets =
        getPartitionHighAndLowOffsets(Topic->topic(), Partition->id());
    std::stringstream Replicas;
    std::copy(Partition->replicas()->begin(), Partition->replicas()->end(),
              std::ostream_iterator<int32_t>(Replicas, ", "));
    std::stringstream ISRSs;
    std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
              std::ostream_iterator<int32_t>(ISRSs, ", "));
  }
  Topics = std::move(TopicsList);
}
}
