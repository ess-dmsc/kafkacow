#pragma once
#include "KafkaMessageMetadataStruct.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

class JSONPrinting {
private:
  void recursiveTruncateJSONMap(YAML::Node &Node);
  void recursiveTruncateJSONSequence(YAML::Node &Node);

public:
  void printTruncatedMessage(const std::string &JSONMessage);
  void printEntireMessage(const std::string &JSONMessage);
};
