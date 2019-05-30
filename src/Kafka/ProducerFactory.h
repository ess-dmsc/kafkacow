#pragma once

#include "Producer.h"
#include <memory>

namespace Kafka {
std::unique_ptr<ProducerInterface> createProducer(const std::string &Broker,
                                                  const std::string Topic,
                                                  bool Real = true);
}
