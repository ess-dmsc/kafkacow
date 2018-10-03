#include "JSONPrinting.h"
#include <algorithm>
#include <fmt/format.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
/// Receives deserialized flatbuffers message, formats it and prints it to the
/// screen.
///
/// \param JSONMessage
std::string getEntireMessage(const std::string &JSONMessage) {
  using std::cout;
  using nlohmann::json;

  // convert into valid JSON
  YAML::Node node = YAML::Load(JSONMessage);
  YAML::Emitter Emitter;
  Emitter << YAML::DoubleQuoted << YAML::Flow << node;
  auto JSONModernMessage = json::parse(Emitter.c_str());
  std::string MessageWithNoQuotes = JSONModernMessage.dump(4);
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
std::string getTruncatedMessage(const std::string &JSONMessage) {
  using std::cout;
  using nlohmann::json;

  YAML::Node Node = truncateMessage(JSONMessage);
  YAML::Emitter Emitter;
  Emitter << YAML::DoubleQuoted << YAML::Flow << Node;
  auto JSONModernMessage = json::parse(Emitter.c_str());
  std::string MessageWithNoQuotes = JSONModernMessage.dump(4);
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), '\"'),
      MessageWithNoQuotes.end());
  MessageWithNoQuotes.erase(
      std::remove(MessageWithNoQuotes.begin(), MessageWithNoQuotes.end(), ','),
      MessageWithNoQuotes.end());
  return MessageWithNoQuotes;
}

/// Initializes truncating and returns edited YAML node.
///
/// \param JSONMessage
/// \return
YAML::Node truncateMessage(const std::string &JSONMessage) {
  YAML::Node Node = YAML::Load(JSONMessage);
  if (Node.IsMap())
    recursiveTruncateJSONMap(Node);
  else if (Node.IsSequence())
    recursiveTruncateJSONSequence(Node);
  return Node;
}

/// Recursive method that receives YAML node and truncates
/// long arrays that it contains.
///
/// \param Node
void recursiveTruncateJSONMap(YAML::Node &Node) {
  for (YAML::const_iterator it = Node.begin(); it != Node.end(); ++it) {
    auto childNode = *it;
    if (it->second.IsMap()) {
      recursiveTruncateJSONMap(childNode.second);
    } else if (it->second.IsSequence()) {
      recursiveTruncateJSONSequence(childNode.second);
    }
  }
}

/// Recursive method that receives YAML node and truncates
/// long arrays that it contains.
///
/// \param Node
void recursiveTruncateJSONSequence(YAML::Node &Node) {
  int Counter = 0;
  size_t OriginalSize = Node.size();
  auto Begin = Node.begin();
  auto End = Node.end();
  for (YAML::const_iterator it = Begin; it != End; ++it) {

    auto childNode = *it;
    if (childNode.IsMap()) {

      recursiveTruncateJSONMap(childNode);
    } else if (childNode.IsSequence()) {

      recursiveTruncateJSONSequence(childNode);
    } else {
      Counter++;
      size_t NodeSize = Node.size();
      if (NodeSize - Counter > 9) {
        Node.remove(NodeSize - Counter);

      } else if (NodeSize - Counter == 0 && OriginalSize > 10) {
        Node.push_back("...");
        Node.push_back(fmt::format("Truncated {} elements.", NodeSize - 10));
      }
    }
  }
}

/// Prints the argument message and a separating line of underscores.
///
/// \param Message
void printToScreen(const std::string &Message) {
  std::cout << Message;
  std::cout << "\n__________________________________________________\n";
}
