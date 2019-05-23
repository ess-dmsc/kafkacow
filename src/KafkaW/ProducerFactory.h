#pragma once

#include "Producer.h"
#include <memory>

namespace KafkaW {
std::unique_ptr<ProducerInterface> createProducer(const std::string &Broker,
                                                  bool Real = true);
}