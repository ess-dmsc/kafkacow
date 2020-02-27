// SPDX-License-Identifier: BSD-2-Clause
//
// This code has been produced by the European Spallation Source
// and its partner institutes under the BSD 2 Clause License.
//
// See LICENSE.md at the top level for license information.
//
// Screaming Udder!                              https://esss.se

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace RdKafka {
class Metadata;
} // RdKafka

namespace Kafka {
class Consumer;
} // RdKafka

namespace Metadata {
struct Broker {
  int32_t ID;
  std::string Host;
  int32_t Port;
};

struct Partition {
  int64_t LowOffset;
  int64_t HighOffset;
  int32_t ID;
};

struct Topic {
  std::string Name;
  std::vector<Partition> Partitions;
};

class Cluster {
public:
  Cluster(Kafka::Consumer const &Consumer,
          std::unique_ptr<RdKafka::Metadata> const &KafkaMetadata);
  std::vector<Broker> Brokers;
  std::vector<Topic> Topics;
};
} // Metadata
