#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace {
void setConfig(RdKafka::Conf &conf, const std::string &ConfigName,
               const std::string &ConfigValue) {
  std::shared_ptr<spdlog::logger> Logger = spdlog::get("LOG");
  std::string ErrStr;
  conf.set(ConfigName, ConfigValue, ErrStr);
  if (!ErrStr.empty()) {
    ErrStr.append(" in createGlobalConfiguration([...])");
    Logger->error(ErrStr);
  }
}

std::unique_ptr<RdKafka::Conf>
createGlobalConfiguration(const std::string &BrokerAddr) {
  auto conf = std::unique_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

  setConfig(*conf, "metadata.broker.list", BrokerAddr);
  setConfig(*conf, "session.timeout.ms", "10000");
  setConfig(*conf, "message.max.bytes", "10000000");
  setConfig(*conf, "fetch.message.max.bytes", "10000000");
  setConfig(*conf, "enable.auto.commit", "false");
  setConfig(*conf, "enable.auto.offset.store", "false");
  setConfig(*conf, "offset.store.method", "none");
  setConfig(*conf, "api.version.request", "true");
  setConfig(*conf, "auto.offset.reset", "largest");
  // kafkacow uses assign not subscribe, so group id is not used for consumer
  // balancing.
  setConfig(*conf, "group.id", "kafkacow");

  return conf;
}
}
