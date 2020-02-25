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
  int32_t const ID;
  std::string const Host;
  int32_t const Port;
};

struct Partition {
  int64_t const LowOffset;
  int64_t const HighOffset;
  int32_t const ID;
};

struct Topic {
  std::string const Name;
  std::vector<Partition> const Partitions;
};

class Cluster {
public:
  Cluster(std::unique_ptr<Kafka::Consumer> const &Consumer,
          std::unique_ptr<RdKafka::Metadata> const &KafkaMetadata);
  std::vector<Broker> Brokers;
  std::vector<Topic> Topics;
};
} // Metadata
