#include "JSONPrinting.h"
#include <ciso646>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace {
size_t STRING_TRUNCATION_LENGTH{50};
size_t ARRAY_TRUNCATION_LENGTH{3};

std::string truncateNONJSON(const std::string &Message) {
  if (Message.size() > STRING_TRUNCATION_LENGTH)
    return Message.substr(0, STRING_TRUNCATION_LENGTH);
  return Message;
}

void truncateJSONString(nlohmann::json &JSONMessage) {
  const auto StringValue = JSONMessage.get<std::string>();
  if (StringValue.size() > STRING_TRUNCATION_LENGTH) {
    const auto TruncatedStringValue =
        StringValue.substr(0, STRING_TRUNCATION_LENGTH);
    JSONMessage = TruncatedStringValue +
                  fmt::format(" ... Truncated {} characters.",
                              StringValue.size() - STRING_TRUNCATION_LENGTH);
  }
}
} // namespace

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
/// \param Indent - number of characters of whitespace to use for indentation(2
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
    bool InArray{false};
    for (auto &Character : MessageWithNoQuotes) {
      if (Character == '[') {
        InArray = true;
      } else if (Character == ']') {
        InArray = false;
      }
      if (InArray and Character == '\n') {
        Character = ' ';
      }
    }
    return MessageWithNoQuotes;

  } catch (nlohmann::json::exception &) {
    return truncateNONJSON(JSONMessage);
  }
}

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

void recursiveTruncateJSONArray(nlohmann::json &JSONMessage) {
  const size_t MessageSize = JSONMessage.size();
  if (MessageSize > ARRAY_TRUNCATION_LENGTH + 1) {
    nlohmann::json::iterator it = JSONMessage.end();
    --it;
    auto last = *it;
    JSONMessage.erase(JSONMessage.begin() + ARRAY_TRUNCATION_LENGTH,
                      JSONMessage.end());
    JSONMessage.push_back(fmt::format("... truncated {} elements ...",
                                      MessageSize - ARRAY_TRUNCATION_LENGTH));
    JSONMessage.push_back(last);
  }

  for (auto &element : JSONMessage) {
    recursiveTruncate(element);
  }
}
