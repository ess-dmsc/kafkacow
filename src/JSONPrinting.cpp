#include "JSONPrinting.h"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace {
size_t TRUNCATE_STRING_TO_LENGTH{50};

std::string truncateNONJSON(const std::string &Message) {
  if (Message.size() > TRUNCATE_STRING_TO_LENGTH)
    return Message.substr(0, TRUNCATE_STRING_TO_LENGTH);
  return Message;
}

void truncateJSONString(nlohmann::json &JSONMessage) {
  auto StringValue = JSONMessage.get<std::string>();
  const auto TruncatedStringValue =
      StringValue.substr(0, TRUNCATE_STRING_TO_LENGTH);
  JSONMessage = TruncatedStringValue;
}
}

/// Receives deserialized flatbuffers message, removes quotes and adds
/// indentation for readability.
///
/// \param JSONMessage
/// \param Indent - number of characters of whitespace to use for indentation(4
/// by default)
/// \return readable message as string
std::string getEntireMessage(const std::string &JSONMessage,
                             const int &Indent) {
  using nlohmann::json;
  try {
    auto JSONModernMessage = json::parse(JSONMessage.c_str());
    std::string MessageWithNoQuotes = JSONModernMessage.dump(Indent);
    MessageWithNoQuotes.erase(std::remove(MessageWithNoQuotes.begin(),
                                          MessageWithNoQuotes.end(), '\"'),
                              MessageWithNoQuotes.end());
    MessageWithNoQuotes.erase(std::remove(MessageWithNoQuotes.begin(),
                                          MessageWithNoQuotes.end(), ','),
                              MessageWithNoQuotes.end());
    return MessageWithNoQuotes;
  } catch (nlohmann::json::exception &) {
    return JSONMessage;
  }
}

/// Receives deserialized flatbuffers message, truncates, removes quotes and
/// adds indentation for readability.
///
/// \param JSONMessage
/// \param Indent - number of characters of whitespace to use for indentation(4
/// by default)
/// \return truncated readable message as string
std::string getTruncatedMessage(const std::string &JSONMessage,
                                const int &Indent) {
  try {
    auto JSONModernMessage = nlohmann::json::parse(JSONMessage.c_str());
    recursiveTruncate(JSONModernMessage);
    std::string MessageWithNoQuotes = JSONModernMessage.dump(Indent);
    MessageWithNoQuotes.erase(std::remove(MessageWithNoQuotes.begin(),
                                          MessageWithNoQuotes.end(), '\"'),
                              MessageWithNoQuotes.end());
    MessageWithNoQuotes.erase(std::remove(MessageWithNoQuotes.begin(),
                                          MessageWithNoQuotes.end(), ','),
                              MessageWithNoQuotes.end());
    return MessageWithNoQuotes;

  } catch (nlohmann::json::exception &) {
    return truncateNONJSON(JSONMessage);
  }
}

/// Recursive method that receives JSON and truncates long arrays that it
/// contains.
///
/// \param JSONMessage
void recursiveTruncateJSONMap(nlohmann::json &JSONMessage) {
  for (nlohmann::json::iterator it = JSONMessage.begin();
       it != JSONMessage.end(); ++it) {
    if (!it->empty()) {
      recursiveTruncate(it.value());
    }
  }
}

void recursiveTruncate(nlohmann::json &JSONMessage) {
  if (JSONMessage.is_string()) {
    truncateJSONString(JSONMessage);
  } else if (JSONMessage.is_array()) {
    recursiveTruncateJSONArray(JSONMessage);
  } else if (JSONMessage.is_object()) {
    recursiveTruncateJSONMap(JSONMessage);
  }
}

/// Recursive method that receives JSON and truncates long arrays that it
/// contains.
///
/// \param JSONMessage
void recursiveTruncateJSONArray(nlohmann::json &JSONMessage) {
  size_t OriginalSize = JSONMessage.size();
  for (nlohmann::json::iterator it = JSONMessage.end() - 1;
       it != JSONMessage.begin(); --it) {
    auto childNode = *it;

    if (childNode.is_object() && !childNode.empty()) {
      recursiveTruncateJSONMap(it.value());
    } else if (childNode.is_array() && !childNode.empty()) {
      recursiveTruncateJSONArray(it.value());
    } else {
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
