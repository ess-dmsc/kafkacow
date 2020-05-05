#include "ProducerInterface.h"
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Kafka {

class FakeProducer : public ProducerInterface {
public:
  FakeProducer(){};

  void produce(Kafka::Message Message) override;

private:
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
};
} // namespace Kafka
