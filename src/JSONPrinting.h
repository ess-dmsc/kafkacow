#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

std::string getEntireMessage(const std::string &JSONMessage);
std::string getTruncatedMessage(const std::string &JSONMessage);
void recursiveTruncateJSONMap(YAML::Node &Node);
void recursiveTruncateJSONSequence(YAML::Node &Node);
YAML::Node truncateMessage(const std::string &JSONMessage);
void printToScreen(const std::string &Message);