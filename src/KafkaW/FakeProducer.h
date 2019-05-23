#include "ProducerInterface.h"
#include <memory>
#include <spdlog/logger.h>

class FakeProducer : public ProducerInterface {
public:
  FakeProducer(){};
  void produce(KafkaW::Message &Message) override;
};
