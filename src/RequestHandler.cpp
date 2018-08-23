#include "RequestHandler.h"
#include "ConnectKafka.h"
#include <thread>
int RequestHandler::init() {
  std::string Topic = "MULTIPART_events";

  std::cout << "_________" << std::endl
            << KafkaConnection->getAllTopics() << std::endl
            << "___________" << std::endl;

  std::vector<std::string> ToSubscribe;
  if (KafkaConnection->checkIfTopicExists(Topic)) {
    ToSubscribe.push_back(Topic);
    for (auto &SingleStruct : KafkaConnection->getHighAndLowOffsets(Topic)) {
      std::cout << SingleStruct.PartitionId << " " << SingleStruct.LowOffset
                << " " << SingleStruct.HighOffset << std::endl;
    }
    subscribeConsumeAtOffset(Topic, 6000);
    std::cout << "##################\n##################\n##################\n#"
                 "#################\n";
    subscribeConsumeNLastMessages(Topic, 3);
  } else
    std::cout << "No such topic" << std::endl;
  return 0;
}

std::string RequestHandler::subscribeConsumeAtOffset(std::string TopicName,
                                                     int64_t Offset) {
  int64_t EOFPartitionCounter = 0,
          NumberOfPartitions =
              KafkaConnection->getNumberOfTopicPartitions(TopicName);
  std::pair<std::string, bool> MessageAndEOF;
  int i = 0;
  // SUBSCRIBE AT AN OFFSET
  KafkaConnection->subscribeAtOffset(Offset, TopicName);
  while (EOFPartitionCounter < NumberOfPartitions) {
    MessageAndEOF = KafkaConnection->consumeFromOffset();
    i++;
    if (MessageAndEOF.second) {
      EOFPartitionCounter++;
      std::cout << "__________________________" << EOFPartitionCounter
                << std::endl;
    }
    std::cout << MessageAndEOF.first << std::endl;
    std::cout << i << std::endl;
  }
  return MessageAndEOF.first;
}

std::string
RequestHandler::subscribeConsumeNLastMessages(std::string TopicName,
                                              int64_t NumberOfMessages) {
  int EOFPartitionCounter = 0,
      NumberOfPartitions =
          KafkaConnection->getNumberOfTopicPartitions(TopicName);
  std::pair<std::string, bool> MessageAndEOF;
  int i = 0;
  KafkaConnection->subscribeToLastNMessages(NumberOfMessages, TopicName);
  while (EOFPartitionCounter < NumberOfPartitions) {
    MessageAndEOF = KafkaConnection->consumeLastNMessages();
    i++;
    if (MessageAndEOF.second) {
      EOFPartitionCounter++;
      std::cout << "__________________________" << EOFPartitionCounter
                << std::endl;
    }
    std::cout << MessageAndEOF.first << std::endl;
    std::cout << i << std::endl;
  }
  return MessageAndEOF.first;
}
