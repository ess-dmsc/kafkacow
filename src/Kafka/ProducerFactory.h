#pragma once

#include "Producer.h"
#include <map>
#include <memory>

namespace Kafka {
std::unique_ptr<ProducerInterface>
createProducer(const std::string &Broker, const std::string &Topic,
               const std::map<std::string, std::string> &KafkaConfiguration,
               bool Real = true);
}
