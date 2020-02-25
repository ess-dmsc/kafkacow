#include "Metadata.h"
#include "Kafka/Consumer.h"

#include <librdkafka/rdkafkacpp.h>

namespace Metadata {
Cluster::Cluster(std::unique_ptr<Kafka::Consumer> const &Consumer,
                 std::unique_ptr<RdKafka::Metadata> const &KafkaMetadata) {
  std::vector<Broker> BrokersList;
  BrokersList.reserve(KafkaMetadata->brokers()->size());
  for (auto KafkaBroker : *KafkaMetadata->brokers()) {
    BrokersList.insert(
        BrokersList.cbegin() + static_cast<size_t>(KafkaBroker->id()),
        {KafkaBroker->id(), KafkaBroker->host(), KafkaBroker->port()});
  }
  Brokers = std::move(BrokersList);

  std::vector<Topic> TopicsList;
  TopicsList.reserve(KafkaMetadata->topics()->size());
  for (auto KafkaTopic : *KafkaMetadata->topics()) {
    std::vector<Partition> PartitionsList;
    PartitionsList.reserve(KafkaTopic->partitions()->size());
    for (auto Partition : *KafkaTopic->partitions()) {
      PartitionsList.insert(
          PartitionsList.cbegin() + static_cast<size_t>(Partition->id()),
          Consumer->getPartitionHighAndLowOffsets(KafkaTopic->topic(),
                                                  Partition->id()));
    }

    TopicsList.push_back({KafkaTopic->topic(), PartitionsList});

    //    std::stringstream Replicas;
    //    std::copy(Partition->replicas()->begin(),
    //    Partition->replicas()->end(),
    //              std::ostream_iterator<int32_t>(Replicas, ", "));
    //    std::stringstream ISRSs;
    //    std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
    //              std::ostream_iterator<int32_t>(ISRSs, ", "));
  }
  Topics = std::move(TopicsList);
}
}
