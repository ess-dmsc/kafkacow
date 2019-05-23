#include "ProducerFactory.h"
#include "FakeProducer.h"
#include "ProducerInterface.h"

std::unique_ptr<ProducerInterface>
KafkaW::createProducer(const std::string &Broker, bool Real) {
  if (Real) {
    return std::make_unique<Producer>(Broker);
  } else {
    return std::make_unique<FakeProducer>(FakeProducer());
  }
}
