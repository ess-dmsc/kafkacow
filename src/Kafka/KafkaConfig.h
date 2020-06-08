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

std::unique_ptr<RdKafka::Conf> createGlobalConfiguration(
    const std::string &BrokerAddr,
    const std::map<std::string, std::string> &KafkaConfiguration) {
  auto conf = std::unique_ptr<RdKafka::Conf>(
      RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

  for (const auto &ConfigItem : KafkaConfiguration) {
    setConfig(*conf, ConfigItem.first, ConfigItem.second);
  }
  setConfig(*conf, "metadata.broker.list", BrokerAddr);
  // kafkacow uses assign not subscribe, so group id is not used for consumer
  // balancing.
  setConfig(*conf, "group.id", "kafkacow");

  return conf;
}
} // namespace
