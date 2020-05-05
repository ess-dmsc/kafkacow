#pragma once

#include "UserArgumentsStruct.h"
#include <nlohmann/json.hpp>
#include <string>

std::string getEntireMessage(const std::string &JSONMessage, const int &Indent);

std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent, bool CompactMessage);

void recursiveTruncate(nlohmann::json &JSONMessage);

void recursiveTruncateJSONMap(nlohmann::json &JSONMessage);

void recursiveTruncateJSONArray(nlohmann::json &JSONMessage);

void recursiveCompactJSONArray(nlohmann::json &JSONMessage);
