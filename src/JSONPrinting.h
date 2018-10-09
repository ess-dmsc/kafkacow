#pragma once

#include "UserArgumentsStruct.h"
#include <string>
#include <yaml-cpp/yaml.h>

std::string getEntireMessage(const std::string &JSONMessage, const int &Indent);

std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent);

void recursiveTruncateJSONMap(YAML::Node &Node);

void recursiveTruncateJSONSequence(YAML::Node &Node);

YAML::Node truncateMessage(const std::string &JSONMessage);
