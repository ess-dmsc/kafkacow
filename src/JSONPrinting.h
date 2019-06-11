#pragma once

#include "UserArgumentsStruct.h"
#include <nlohmann/json.hpp>
#include <string>

std::string getEntireMessage(const std::string &JSONMessage, const int &Indent);

std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent);

void recursiveTruncateJSONMap(nlohmann::json &JSONMessage);

void recursiveTruncateJSONSequence(nlohmann::json &JSONMessage);

std::string truncateNONJSON(const std::string &Message);
