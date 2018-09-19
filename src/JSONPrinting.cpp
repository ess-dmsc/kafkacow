#include "JSONPrinting.h"
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

void JSONPrinting::printEntireMessage(const std::string &JSONMessage) {

  using std::cout;
  using nlohmann::json;

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
  cout << MessageWithNoQuotes;
  cout << "\n__________________________________________________\n";
}

void JSONPrinting::printTruncatedMessage(const std::string &JSONMessage) {
  using std::cout;
  using nlohmann::json;

  YAML::Node node = YAML::Load(JSONMessage);
  if (node.IsMap())
    recursiveTruncateJSONMap(node);
  else if (node.IsSequence())
    recursiveTruncateJSONSequence(node);

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
  cout << MessageWithNoQuotes;
  cout << "\n__________________________________________________\n";
}

void JSONPrinting::recursiveTruncateJSONMap(YAML::Node &Node) {
  for (YAML::const_iterator it = Node.begin(); it != Node.end(); ++it) {
    std::vector<std::string> Values;
    Values.push_back(it->first.as<std::string>());
    auto childNode = *it;
    if (it->second.IsMap()) {
      recursiveTruncateJSONMap(childNode.second);
    } else if (it->second.IsSequence()) {
      recursiveTruncateJSONSequence(childNode.second);
    } else {
      Values.push_back(it->second.as<std::string>());
    }
  }
}

void JSONPrinting::recursiveTruncateJSONSequence(YAML::Node &Node) {
  std::vector<std::string> Values;
  int Counter = 0;
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
      int NodeSize = Node.size();
      if (NodeSize - Counter > 10) {
        Node.remove(NodeSize - Counter);

      } else if (NodeSize - Counter == 0) {
        Node.remove(NodeSize - Counter);
        std::stringstream ss;
        Node.push_back("[...]");
        ss << "Ommitted " << NodeSize - 10 << " results.";
        Node.push_back(ss.str());
      }
    }
  }
}
