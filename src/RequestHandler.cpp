#include "RequestHandler.h"
#include "ArgumentsException.h"
#include "JSONPrinting.h"
#include <fmt/format.h>

/// Analyzes user arguments, checks which mode(consumer/metadata) is chosen and
/// calls method responsible for handling one of the modes or throws
/// ArgumentsException if arguments invalid.
/// \param UserArguments
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

/// Analyzes user arguments to determine which consumer mode functionality to
/// run.
///
/// \param UserArguments
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

/// Analyzes user arguments to determine which metadata mode functionality to
/// run.
///
/// \param UserArguments
void RequestHandler::checkMetadataModeArguments(
    UserArgumentStruct UserArguments) {
  if (UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->getAllTopics() << "\n";
  else if (!UserArguments.Name.empty())
    showTopicPartitionOffsets(UserArguments);
  else if (!UserArguments.ShowAllTopics)
    std::cout << KafkaConnection->showAllMetadata();
}

/// Subscribes at an offset to a specified TopicName and consumes the data.
///
/// \param TopicName
/// \param Offset
void RequestHandler::subscribeConsumeAtOffset(std::string TopicName,
                                              int64_t Offset) {
  verifyOffset(Offset, TopicName);

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

/// Ensures there are messages to read at offset provided by the user, otherwise
/// throws an ArgumentsException.
///
/// \param Offset
/// \param TopicName
void RequestHandler::verifyOffset(const int64_t Offset,
                                  const std::string TopicName) {
  std::vector<OffsetsStruct> Offsets =
      KafkaConnection->getTopicsHighAndLowOffsets(TopicName);
  bool InvalidOffset = true;
  for (OffsetsStruct Struct : Offsets) {
    if (Offset < Struct.HighOffset && Offset > Struct.LowOffset) {
      InvalidOffset = false;
      break;
    }
  }
  if (InvalidOffset)
    throw ArgumentsException("Offset not valid!");
}

/// Subscribes to NumberOfMessages from Partition of specified TopicName and
/// consumes the data.
///
/// \param TopicName
/// \param NumberOfMessages
/// \param Partition
void RequestHandler::subscribeConsumeNLastMessages(std::string TopicName,
                                                   int64_t NumberOfMessages,
                                                   int Partition) {
  verifyNLast(NumberOfMessages, TopicName, Partition);
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

/// Checks if there is enough messages to read on a specified partition.
/// Otherwise throws an ArgumentsException.
///
/// \param NLast
/// \param TopicName
/// \param Partition
void RequestHandler::verifyNLast(const int64_t NLast,
                                 const std::string TopicName,
                                 const int16_t Partition) {
  OffsetsStruct Struct =
      KafkaConnection->getPartitionHighAndLowOffsets(TopicName, Partition);
  if (Struct.HighOffset - Struct.LowOffset < NLast)
    throw ArgumentsException("Cannot display that many messages!");
}

/// Prints to screen a list of partitions' IDs and their low/high offsets.
///
/// \param UserArguments
void RequestHandler::showTopicPartitionOffsets(
    UserArgumentStruct UserArguments) {
  std::cout << UserArguments.Name << "\n";
  for (auto &SingleStruct :
       KafkaConnection->getTopicsHighAndLowOffsets(UserArguments.Name)) {
    fmt::print("Partition ID: {} || Low offset: {} || High offset: {}",
               SingleStruct.PartitionId, SingleStruct.LowOffset,
               SingleStruct.HighOffset);
  }
}

/// Receives a serialized message and if it is not empty, passes it to
/// FlatbuffersTranslator to deserialize.
///
/// \param MessageAndEOF
/// \param EOFPartitionCounter
/// \param FlatBuffers
void RequestHandler::consumePartitions(KafkaMessageMetadataStruct &MessageData,
                                       int &EOFPartitionCounter,
                                       FlatbuffersTranslator &FlatBuffers) {
  if (!MessageData.Payload.empty()) {
    std::string JSONMessage = FlatBuffers.deserializeToYAML(MessageData);
    printMessageMetadata(MessageData);
    (UserArguments.ShowEntireMessage)
        ? std::cout << fmt::format("{}", getEntireMessage(JSONMessage))
        : std::cout << fmt::format("{}", getTruncatedMessage(JSONMessage));
  }
  if (MessageData.PartitionEOF)
    EOFPartitionCounter++;
}

void RequestHandler::printMessageMetadata(
    KafkaMessageMetadataStruct &MessageData) {
  std::cout << fmt::format("\n{:_>67}{:>67}\nTimestamp: {:>11} || PartitionID: "
                           "{:>5} || Offset: {:>7}\n",
                           "\n", "|", MessageData.Timestamp,
                           MessageData.Partition, MessageData.Offset);
}
