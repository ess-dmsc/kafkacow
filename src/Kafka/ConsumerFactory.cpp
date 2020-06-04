#include "ConsumerFactory.h"
#include "FakeConsumer.h"

namespace Kafka {

std::unique_ptr<ConsumerInterface>
createConsumer(const std::string &Broker,
               const std::map<std::string, std::string> &KafkaConfiguration,
               bool Real) {
  if (Real) {
    return std::make_unique<Consumer>(Broker, KafkaConfiguration);
  } else {
    return std::make_unique<FakeConsumer>(FakeConsumer());
  }
}
} // namespace Kafka
