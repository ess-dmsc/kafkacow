#include "RequestHandler.h"
#include "ArgumentsException.h"
#include "JSONPrinting.h"

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
  if (UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->getAllTopics() << "\n";
  else if (!UserArguments.Name.empty())
    showTopicPartitionOffsets(UserArguments);
  else if (!UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->showAllMetadata();
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
  std::cout << UserArguments.Name << "\n";
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
  if (!MessageData.Payload.empty()) {
    std::string JSONMessage = FlatBuffers.translateToJSON(MessageData);
    (UserArguments.ShowEntireMessage)
        ? printToScreen(getEntireMessage(JSONMessage))
        : printToScreen(getTruncatedMessage(JSONMessage));
  }
  if (MessageData.PartitionEOF)
    EOFPartitionCounter++;
}
