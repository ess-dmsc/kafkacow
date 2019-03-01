#include "JSONPrinting.h"
#include <algorithm>
#include <fmt/format.h>
#include <iostream>
#include <nlohmann/json.hpp>

/// Receives deserialized flatbuffers message, formats it and prints it to the
/// screen.
///
/// \param JSONMessage
std::string getEntireMessage(const std::string &JSONMessage,
                             const int &Indent) {
  using nlohmann::json;
  auto JSONModernMessage = json::parse(JSONMessage.c_str());
  std::string MessageWithNoQuotes = JSONModernMessage.dump(Indent);
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), '\"'),
      MessageWithNoQuotes.end());
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), ','),
      MessageWithNoQuotes.end());
  return MessageWithNoQuotes;
}

/// Receives deserialized flatbuffers message, truncated large arrays, formats
/// it and prints it to the screen.
///
/// \param JSONMessage
std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent) {

  auto JSONModernMessage = nlohmann::json::parse(JSONMessage.c_str());
  if (JSONModernMessage.is_array()) {
    recursiveTruncateJSONSequence(JSONModernMessage);
  } else if (JSONModernMessage.is_object()) {
    recursiveTruncateJSONMap(JSONModernMessage);
  }
  std::string MessageWithNoQuotes = JSONModernMessage.dump(Indent);
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), '\"'),
      MessageWithNoQuotes.end());
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), ','),
      MessageWithNoQuotes.end());
  return MessageWithNoQuotes;
}

/// Recursive method that truncates
/// long arrays that it contains.
///
/// \param Node
void recursiveTruncateJSONMap(nlohmann::json &JSONMessage) {
  for (nlohmann::json::iterator it = JSONMessage.begin();
       it != JSONMessage.end(); ++it) {
    if (it.value().is_object()) {
      recursiveTruncateJSONMap(it.value());
    } else if (it.value().is_array()) {
      recursiveTruncateJSONSequence(it.value());
    }
  }
}

/// Recursive method that truncates
/// long arrays that it contains.
///
/// \param Node
void recursiveTruncateJSONSequence(nlohmann::json &JSONMessage) {
  int Counter = 0;
  size_t OriginalSize = JSONMessage.size();
  for (nlohmann::json::iterator it = JSONMessage.end() - 1;
       it != JSONMessage.begin(); --it) {
    auto childNode = *it;
    if (childNode.is_object()) {
      recursiveTruncateJSONMap(it.value());
    } else if (childNode.is_array()) {
      recursiveTruncateJSONSequence(it.value());
    } else {
      Counter++;
      size_t MessageSize = JSONMessage.size();
      if (MessageSize > 10) {
        JSONMessage.erase(MessageSize - 1);
      } else if (MessageSize == 10 && OriginalSize > 10) {
        JSONMessage.push_back("...");
        JSONMessage.push_back(
            fmt::format("Truncated {} elements.", OriginalSize - 10));
        return;
      }
    }
  }
}
