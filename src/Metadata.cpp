#include "Metadata.h"
#include "Kafka/Consumer.h"

#include <librdkafka/rdkafkacpp.h>

namespace Metadata {
Cluster::Cluster(Kafka::Consumer const &Consumer,
                 std::unique_ptr<RdKafka::Metadata> const &KafkaMetadata) {
  for (auto KafkaBroker : *KafkaMetadata->brokers()) {
    Brokers.push_back(
        {KafkaBroker->id(), KafkaBroker->host(), KafkaBroker->port()});
  }

  Topics.reserve(KafkaMetadata->topics()->size());
  for (auto KafkaTopic : *KafkaMetadata->topics()) {
    std::vector<Partition> PartitionsList;
    PartitionsList.reserve(KafkaTopic->partitions()->size());
    for (auto Partition : *KafkaTopic->partitions()) {
      PartitionsList.insert(
          PartitionsList.cbegin() + static_cast<size_t>(Partition->id()),
          Consumer.getPartitionHighAndLowOffsets(KafkaTopic->topic(),
                                                 Partition->id()));
    }

    Topics.push_back({KafkaTopic->topic(), PartitionsList});

    //    std::stringstream Replicas;
    //    std::copy(Partition->replicas()->begin(),
    //    Partition->replicas()->end(),
    //              std::ostream_iterator<int32_t>(Replicas, ", "));
    //    std::stringstream ISRSs;
    //    std::copy(Partition->isrs()->begin(), Partition->isrs()->end(),
    //              std::ostream_iterator<int32_t>(ISRSs, ", "));
  }
}
}
