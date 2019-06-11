#include "ConsumerFactory.h"
#include "FakeConsumer.h"

namespace Kafka {

std::unique_ptr<ConsumerInterface> createConsumer(const std::string &Broker,
                                                  bool Real) {
  if (Real) {
    return std::make_unique<Consumer>(Broker);
  } else {
    return std::make_unique<FakeConsumer>(FakeConsumer());
  }
}
}
