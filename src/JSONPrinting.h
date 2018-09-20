#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

void printEntireMessage(const std::string &JSONMessage);
void printTruncatedMessage(const std::string &JSONMessage);
void recursiveTruncateJSONMap(YAML::Node &Node);
void recursiveTruncateJSONSequence(YAML::Node &Node);
YAML::Node truncateMessage(const std::string &JSONMessage);
