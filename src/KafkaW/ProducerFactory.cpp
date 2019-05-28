#include "ProducerFactory.h"
#include "FakeProducer.h"
#include "ProducerInterface.h"

std::unique_ptr<ProducerInterface>
KafkaW::createProducer(const std::string &Broker, const std::string Topic,
                       bool Real) {
  if (Real) {
    return std::make_unique<Producer>(Broker, Topic);
  } else {
    return std::make_unique<FakeProducer>(FakeProducer());
  }
}
