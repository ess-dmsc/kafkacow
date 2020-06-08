#include "ProducerFactory.h"
#include "FakeProducer.h"
#include "ProducerInterface.h"

namespace Kafka {

std::unique_ptr<ProducerInterface>
createProducer(const std::string &Broker, const std::string &Topic,
               const std::map<std::string, std::string> &KafkaConfiguration,
               bool Real) {
  if (Real) {
    return std::make_unique<Producer>(Broker, Topic, KafkaConfiguration);
  } else {
    return std::make_unique<FakeProducer>(FakeProducer());
  }
}
} // namespace Kafka
