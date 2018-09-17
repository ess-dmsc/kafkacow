#include "RequestHandler.h"
#include "ArgumentsException.h"
#include <iomanip>
#include <yaml-cpp/yaml.h>

// check whether arguments passed match any methods
void RequestHandler::checkAndRun() {
  // check input if ConsumerMode chosen
  if (UserArguments.ConsumerMode && !UserArguments.MetadataMode)
    checkConsumerModeArguments(UserArguments);

  // check input if MetadataMode chosen
  else if (!UserArguments.ConsumerMode && UserArguments.MetadataMode)
    checkMetadataModeArguments(UserArguments);
  // no MetadataMode or ConsumerMode chosen
  else
    throw ArgumentsException(
        "Program can run in one and only one mode: --consumer or --metadata");
}

void RequestHandler::checkConsumerModeArguments(
    UserArgumentStruct UserArguments) {
  if ((UserArguments.GoBack > -2 && UserArguments.OffsetToStart > -2) ||
      (UserArguments.GoBack == -2 && UserArguments.OffsetToStart == -2))
    throw ArgumentsException("Program must take one and only one of the "
                             "arguments: \"--go\",\"--Offset\"");
  else {
    if (UserArguments.OffsetToStart > -2) {
      subscribeConsumeAtOffset(UserArguments.Name, UserArguments.OffsetToStart);
    } else {
      (UserArguments.PartitionToConsume != -1)
          ? subscribeConsumeNLastMessages(UserArguments.Name,
                                          UserArguments.GoBack,
                                          UserArguments.PartitionToConsume)
          : Logger->error("Please specify partition");
    }
  }
}

void RequestHandler::checkMetadataModeArguments(
    UserArgumentStruct UserArguments) {
  if (!UserArguments.ShowAllTopics && !UserArguments.ShowPartitionsOffsets)
    throw ArgumentsException("No action specified for \"--list\" mode");
  else if (UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->getAllTopics() << "\n";
  if (UserArguments.ShowPartitionsOffsets)
    showTopicPartitionOffsets(UserArguments);
}

void RequestHandler::subscribeConsumeAtOffset(std::string TopicName,
                                              int64_t Offset) {
  int EOFPartitionCounter = 0;
  int NumberOfPartitions =
      KafkaConnection->getNumberOfTopicPartitions(TopicName);

  KafkaConnection->subscribeAtOffset(Offset, TopicName);
  FlatbuffersTranslator FlatBuffers;
  while (EOFPartitionCounter < NumberOfPartitions) {
    KafkaMessageMetadataStruct MessageData;
    MessageData = KafkaConnection->consumeFromOffset();
    consumePartitions(MessageData, EOFPartitionCounter, FlatBuffers);
  }
}

void RequestHandler::subscribeConsumeNLastMessages(std::string TopicName,
                                                   int64_t NumberOfMessages,
                                                   int Partition) {
  int EOFPartitionCounter = 0;
  KafkaConnection->subscribeToLastNMessages(NumberOfMessages, TopicName,
                                            Partition);
  FlatbuffersTranslator FlatBuffers;
  KafkaMessageMetadataStruct MessageData;
  while (EOFPartitionCounter < 1) {

    MessageData = KafkaConnection->consumeLastNMessages();
    consumePartitions(MessageData, EOFPartitionCounter, FlatBuffers);
  }
}

void RequestHandler::showTopicPartitionOffsets(
    UserArgumentStruct UserArguments) {
  for (auto &SingleStruct :
       KafkaConnection->getHighAndLowOffsets(UserArguments.Name)) {
    std::cout << "Partition ID: " << SingleStruct.PartitionId
              << " || Low offset: " << SingleStruct.LowOffset
              << " || High offset: " << SingleStruct.HighOffset << "\n";
  }
}

void RequestHandler::consumePartitions(KafkaMessageMetadataStruct &MessageData,
                                       int &EOFPartitionCounter,
                                       FlatbuffersTranslator &FlatBuffers) {
  if (!MessageData.PayloadToReturn.empty() &&
      MessageData.PayloadToReturn != "HiddenSecretMessageFromLovingNeutron") {
    std::string JSONMessage = FlatBuffers.getFileID(MessageData);
    (UserArguments.ShowEntireMessage)
        ? printMessage1(JSONMessage, MessageData)
        : printTruncatedMessage(JSONMessage, MessageData);
  }
  if (MessageData.PartitionEOF)
    EOFPartitionCounter++;
}

void RequestHandler::printMessage(const std::string &JSONMessage,
                                  KafkaMessageMetadataStruct MessageData) {
  using std::cout;
  using std::setw;
  std::cout << setw(50) << "- Partition: " << setw(4) << MessageData.Partition
            << "\n"
            << setw(47) << "- Offset: " << setw(7) << MessageData.Offset
            << "\n- Timestamp: " << setw(21) << MessageData.Timestamp << "\n";
  YAML::Node node = YAML::Load(JSONMessage);
  // print message details:
  cout << "- source_name: " << setw(19) << node["source_name"].as<std::string>()
       << "\n- message_id: " << setw(20) << node["message_id"].as<std::string>()
       << "\n- pulse_time: " << setw(20) << node["pulse_time"].as<std::string>()
       << "\n"
       << std::endl;

  cout << "no." << setw(5) << "||" << setw(17) << "time_of_flight:" << setw(3)
       << "||" << setw(15) << "detector_id:\n";
  cout << "______||__________________||_______________\n";
  for (unsigned int i = 0; i < node["time_of_flight"].size(); i++) {
    cout << setw(5) << i << setw(3) << "||" << setw(15)
         << node["time_of_flight"][i].as<std::string>() << setw(5) << "||"
         << setw(10) << node["detector_id"][i].as<std::string>() << "\n";
  }
  cout << "=======================================================\n";
}

void RequestHandler::printMessage1(const std::string &JSONMessage,
                                   KafkaMessageMetadataStruct MessageData) {
  using std::cout;
  using std::setw;

  YAML::Node node = YAML::Load(JSONMessage);
  // print message details:
  std::vector<std::vector<std::string>> Keys;
  if (node.IsMap()) {
    recursivePrintJSONMap(node, Keys);
  } else if (node.IsSequence()) {
    recursivePrintJSONSequence(node, Keys);
  } else
    cout << "chuj\n";
}

void RequestHandler::printTruncatedMessage(
    const std::string &JSONMessage, KafkaMessageMetadataStruct MessageData) {
  using std::cout;
  using std::setw;

  std::cout << setw(50) << "- Partition: " << setw(4) << MessageData.Partition
            << "\n"
            << setw(47) << "- Offset: " << setw(7) << MessageData.Offset
            << "\n- Timestamp: " << setw(21) << MessageData.Timestamp << "\n";
  YAML::Node node = YAML::Load(JSONMessage);
  // print message details:
  cout << "- source_name: " << setw(19) << node["source_name"].as<std::string>()
       << "\n- message_id: " << setw(20) << node["message_id"].as<std::string>()
       << "\n- pulse_time: " << setw(20) << node["pulse_time"].as<std::string>()
       << "\n"
       << std::endl;
  // print truncated values

  cout << "no." << setw(5) << "||" << setw(17) << "time_of_flight:" << setw(3)
       << "||" << setw(15) << "detector_id:\n";
  cout << "______||__________________||_______________\n";

  if (node["time_of_flight"].size() < 10) {
    for (unsigned int i = 0; i < node["time_of_flight"].size(); i++) {
      cout << setw(5) << i << setw(3) << "||" << setw(15)
           << node["time_of_flight"][i].as<std::string>() << setw(5) << "||"
           << setw(10) << node["detector_id"][i].as<std::string>() << "\n";
    }
  } else {
    for (auto i = 0; i < 10; i++) {
      cout << setw(5) << i << setw(3) << "||" << setw(15)
           << node["time_of_flight"][i].as<std::string>() << setw(5) << "||"
           << setw(10) << node["detector_id"][i].as<std::string>() << "\n";
    }
    cout << "[...]" << setw(3) << "||" << setw(15) << "[...]" << setw(5) << "||"
         << setw(11) << "[...]\n";
    cout << "------> Omitted " << node["time_of_flight"].size() - 10
         << " results.\n";
    cout << "=======================================================\n";
  }
}

void RequestHandler::recursivePrintJSONMap(
    YAML::Node &Node, std::vector<std::vector<std::string>> &Keys) {
  for (YAML::const_iterator it = Node.begin(); it != Node.end(); ++it) {
    std::vector<std::string> Values;
    Values.push_back(it->first.as<std::string>());
    std::cout << it->first.as<std::string>() << "  ";
    auto childNode = *it;
    if (it->second.IsMap()) {
      recursivePrintJSONMap(childNode.second, Keys);
    } else if (it->second.IsSequence()) {
      recursivePrintJSONSequence(childNode.second, Keys);
    } else {
      Values.push_back(it->second.as<std::string>());
      Keys.push_back(Values);
      std::cout << it->second.as<std::string>() << "\n";
    }
  }
}

void RequestHandler::recursivePrintJSONSequence(
    YAML::Node &Node, std::vector<std::vector<std::string>> &Keys) {
  std::vector<std::string> Values;
  std::string val;
  for (YAML::const_iterator it = Node.begin(); it != Node.end(); ++it) {

    auto childNode = *it;
    if (childNode.IsMap()) {
      recursivePrintJSONMap(childNode, Keys);
    } else if (childNode.IsSequence()) {
      recursivePrintJSONSequence(childNode, Keys);
    } else {
      val.append(" | ");
      val.append(childNode.as<std::string>());
      Values.push_back(childNode.as<std::string>());
    }
  }
  std::cout << val << "\n";
  Keys.back().insert(std::end(Keys.back()), std::begin(Values),
                     std::end(Values));
}
