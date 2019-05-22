#pragma once

#include "Consumer.h"
#include <memory>

namespace KafkaW {
std::unique_ptr<ConsumerInterface> createConsumer(const std::string &Broker,
                                                  bool Real = true);
}