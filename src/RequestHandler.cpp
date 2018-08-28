#include "RequestHandler.h"
#include "ConnectKafka.h"
#include <thread>

int RequestHandler::init(UserArgumentStruct UserArguments) {
  checkAndRun(UserArguments);
  return 0;
}

// check whether arguments passed match any methods
void RequestHandler::checkAndRun(UserArgumentStruct UserArguments) {
  try {
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
  } catch (ArgumentsException E) {
    E.printException();
  } catch (std::exception E) {
    std::cout << E.what() << std::endl;
  }
}

void RequestHandler::checkConsumerModeArguments(
    UserArgumentStruct UserArguments) {
  if ((UserArguments.GoBack > -2 && UserArguments.OffsetToStart > -2) ||
      (UserArguments.GoBack == -2 && UserArguments.OffsetToStart == -2))
    throw ArgumentsException("Program must take one and only one of the "
                             "arguments: \"--go\",\"--Offset\"");
  else {
    UserArguments.OffsetToStart > -2
        ? subscribeConsumeAtOffset(UserArguments.Name,
                                   UserArguments.OffsetToStart)
        : subscribeConsumeNLastMessages(UserArguments.Name,
                                        UserArguments.GoBack);
  }
}

void RequestHandler::checkMetadataModeArguments(
    UserArgumentStruct UserArguments) {
  if (!UserArguments.ShowAllTopics && !UserArguments.ShowPartitionsOffsets)
    throw ArgumentsException("No action specified for \"--list\" mode");
  else if (UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->getAllTopics() << std::endl;
  if (UserArguments.ShowPartitionsOffsets)
    showTopicPartitionOffsets(UserArguments);
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

void RequestHandler::showTopicPartitionOffsets(
    UserArgumentStruct UserArguments) {
  for (auto &SingleStruct :
       KafkaConnection->getHighAndLowOffsets(UserArguments.Name)) {
    std::cout << SingleStruct.PartitionId << " | " << SingleStruct.LowOffset
              << " | " << SingleStruct.HighOffset << std::endl;
  }
}
