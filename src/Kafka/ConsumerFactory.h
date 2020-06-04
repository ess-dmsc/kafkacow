#pragma once

#include "Consumer.h"
#include <memory>

namespace Kafka {
std::unique_ptr<ConsumerInterface>
createConsumer(const std::string &Broker,
               const std::map<std::string, std::string> &KafkaConfiguration,
               bool Real = true);
}
