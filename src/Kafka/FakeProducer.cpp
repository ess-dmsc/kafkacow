#include "FakeProducer.h"

namespace Kafka {

void FakeProducer::produce(Kafka::Message Message) {
  Logger->debug("FakeProducer::produce()");
}
} // namespace Kafka
