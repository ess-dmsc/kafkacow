#include "RequestHandler.h"
#include "ConnectKafka.h"
#include <thread>
int RequestHandler::Init() {
  std::string Topic = "MULTIPART_events";

  std::cout << "_________" << std::endl
            << KafkaConnection->GetAllTopics() << std::endl
            << "___________" << std::endl;

  std::vector<std::string> ToSubscribe;
  if (KafkaConnection->CheckIfTopicExists(Topic)) {
    ToSubscribe.push_back(Topic);
    for (auto &SingleStruct : KafkaConnection->GetHighAndLowOffsets(Topic)) {
      std::cout << SingleStruct.PartitionId << " " << SingleStruct.LowOffset
                << " " << SingleStruct.HighOffset << std::endl;
    }
    SubscribeConsumeAtOffset(Topic, 6000);
    std::cout << "##################\n##################\n##################\n#"
                 "#################\n";
    SubscribeConsumeNLastMessages(Topic, 3);
  } else
    std::cout << "No such topic" << std::endl;
  return 0;
}

std::string RequestHandler::SubscribeConsumeAtOffset(std::string TopicName,
                                                     int64_t Offset) {
  int EOFPartitionCounter = 0,
      NumberOfPartitions =
          KafkaConnection->GetNumberOfTopicPartitions(TopicName);
  std::pair<std::string, bool> MessageAndEOF;
  int i = 0;
  // SUBSCRIBE AT AN OFFSET
  KafkaConnection->SubscribeAtOffset(Offset, TopicName);
  while (EOFPartitionCounter < NumberOfPartitions) {
    MessageAndEOF = KafkaConnection->ConsumeFromOffset(TopicName);
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
RequestHandler::SubscribeConsumeNLastMessages(std::string TopicName,
                                              int64_t NumberOfMessages) {
  int EOFPartitionCounter = 0,
      NumberOfPartitions =
          KafkaConnection->GetNumberOfTopicPartitions(TopicName);
  std::pair<std::string, bool> MessageAndEOF;
  int i = 0;
  KafkaConnection->SubscribeToLastNMessages(NumberOfMessages, TopicName);
  while (EOFPartitionCounter < NumberOfPartitions) {
    MessageAndEOF =
        KafkaConnection->ConsumeLastNMessages(TopicName, NumberOfMessages);
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
