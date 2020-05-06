#pragma once

#include "UserArgumentsStruct.h"
#include <nlohmann/json.hpp>
#include <string>

std::string getEntireMessage(const std::string &JSONMessage, const int &Indent);

std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent);

void recursiveTruncate(nlohmann::json &JSONMessage);

void recursiveTruncateJSONMap(nlohmann::json &JSONMessage);

void recursiveTruncateJSONArray(nlohmann::json &JSONMessage);
