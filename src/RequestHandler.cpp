#include "RequestHandler.h"
#include "CustomExceptions.h"
#include "JSONPrinting.h"
#include <chrono>
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
    throw ArgumentException(
        "Program can run in one and only one mode: --consumer or --metadata");
}

/// Analyzes user arguments to determine which consumer mode functionality to
/// run.
///
/// \param UserArguments
/// \param TerminateAtEndOfTopic terminate at end of topic. For unit tests.
void RequestHandler::checkConsumerModeArguments(
    UserArgumentStruct UserArguments, bool TerminateAtEndOfTopic) {
  if (UserArguments.GoBack == -2 && UserArguments.OffsetToStart == -2 &&
      UserArguments.Name.empty()) {
    throw ArgumentException("Please specify topic!");
  }
  if (UserArguments.GoBack == -2 && UserArguments.OffsetToStart == -2) {
    printEntireTopic(UserArguments.Name, TerminateAtEndOfTopic);
  } else {
    checkIfTopicEmpty(UserArguments.Name);
    if (UserArguments.GoBack > -2 && UserArguments.OffsetToStart > -2) {
      subscribeAndConsume(UserArguments.Name, UserArguments.GoBack,
                          UserArguments.PartitionToConsume,
                          UserArguments.OffsetToStart);
    } else {
      if (UserArguments.OffsetToStart > -2) {
        subscribeAndConsume(UserArguments.Name, UserArguments.OffsetToStart);
      } else {
        (UserArguments.PartitionToConsume != -1)
            ? subscribeAndConsume(UserArguments.Name, UserArguments.GoBack,
                                  UserArguments.PartitionToConsume)
            : Logger->error("Please specify partition");
      }
    }
  }
}

void RequestHandler::checkIfTopicEmpty(const std::string &TopicName) {
  std::vector<OffsetsStruct> HighAndLowOffsets =
      KafkaConnection->getTopicsHighAndLowOffsets(TopicName);
  bool EmptyTopic = true;
  for (auto Offsets : HighAndLowOffsets) {
    if (Offsets.LowOffset != Offsets.HighOffset)
      EmptyTopic = false;
  }
  if (EmptyTopic)
    throw ArgumentException("Topic is empty!");
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

/// Ensures there are messages to read at offset provided by the user, otherwise
/// throws an ArgumentsException.
///
/// \param Offset
/// \param TopicName
bool RequestHandler::verifyOffset(const int64_t Offset,
                                  const std::string &TopicName) {
  std::vector<OffsetsStruct> Offsets =
      KafkaConnection->getTopicsHighAndLowOffsets(TopicName);
  bool InvalidOffset = true;
  for (OffsetsStruct Struct : Offsets) {
    if (Offset <= Struct.HighOffset && Offset >= Struct.LowOffset) {
      InvalidOffset = false;
      break;
    }
  }
  return InvalidOffset;
}

/// Checks if there is enough messages to read on a specified partition.
/// Otherwise throws an ArgumentsException.
///
/// \param NLast
/// \param TopicName
/// \param Partition
void RequestHandler::verifyNLast(const int64_t NLast,
                                 const std::string &TopicName,
                                 const int16_t Partition) {
  OffsetsStruct Struct =
      KafkaConnection->getPartitionHighAndLowOffsets(TopicName, Partition);
  if (Struct.HighOffset - Struct.LowOffset < NLast)
    throw ArgumentException("Cannot display that many messages!");
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
void RequestHandler::printKafkaMessage(KafkaMessageMetadataStruct &MessageData,
                                       int &EOFPartitionCounter,
                                       FlatbuffersTranslator &FlatBuffers) {
  if (!MessageData.Payload.empty()) {
    std::string FileIdentifier;
    std::string JSONMessage =
            FlatBuffers.deserializeToJSON(MessageData, FileIdentifier);
    printMessageMetadata(MessageData, FileIdentifier);
    (UserArguments.ShowEntireMessage)
        ? std::cout << fmt::format(
              "{}", getEntireMessage(JSONMessage, UserArguments.Indentation))
        : std::cout << fmt::format(
              "{}",
              getTruncatedMessage(JSONMessage, UserArguments.Indentation));
  }
  if (MessageData.PartitionEOF)
    EOFPartitionCounter++;
}

/// Prints and formats message Metadata.
///
/// \param MessageData
void RequestHandler::printMessageMetadata(
    KafkaMessageMetadataStruct &MessageData,
    const std::string &FileIdentifier) {
  std::cout << fmt::format(
      "\n{:_>67}{}{:>39}\n\nTimestamp: {:>11} || PartitionID: "
      "{:>5} || Offset: {:>7} || File Identifier: {}\n",
      "\n", MessageData.TimestampISO, "|", MessageData.Timestamp,
      MessageData.Partition, MessageData.Offset, FileIdentifier);
}

/// Calculates topic's lowest offset and subscribes to it to print the entire
/// topic.
///
/// \param TopicName
/// \param TerminateAtEndOfTopic terminate at end of topic. For unit tests.
void RequestHandler::printEntireTopic(const std::string &TopicName,
                                      bool TerminateAtEndOfTopic) {
  std::vector<OffsetsStruct> OffsetsStruct =
      KafkaConnection->getTopicsHighAndLowOffsets(TopicName);
  int64_t MinOffset = OffsetsStruct[0].LowOffset;
  for (auto OffsetStruct : OffsetsStruct) {
    if (OffsetStruct.LowOffset < MinOffset)
      MinOffset = OffsetStruct.LowOffset;
  }
  subscribeAndConsume(TopicName, MinOffset, TerminateAtEndOfTopic);
}

std::string RequestHandler::timestampToReadable(const int64_t &Timestamp) {
  using namespace std;
  time_t Seconds = Timestamp / 1000;
  ctime(&Seconds);
  istringstream iss(asctime(localtime(&Seconds)));
  vector<string> tokens{istream_iterator<string>{iss},
                        istream_iterator<string>{}};
  return fmt::format("{} {}-{}-{} {}::{}", tokens[0], tokens[2], tokens[1],
                     tokens[4], tokens[3], Timestamp % 1000);
}

/// Subscribes to NumberOfMessages from Partition of specified TopicName and
/// consumes the data.
///
/// \param TopicName
/// \param NumberOfMessages
/// \param Partition
void RequestHandler::subscribeAndConsume(const std::string &TopicName,
                                         int64_t NumberOfMessages,
                                         int Partition) {
  verifyNLast(NumberOfMessages, TopicName, Partition);
  int EOFPartitionCounter = 0;
  KafkaConnection->subscribeToLastNMessages(NumberOfMessages, TopicName,
                                            Partition);
  FlatbuffersTranslator FlatBuffers(SchemaPath);

  while (EOFPartitionCounter < 1) {
    consumeSingleMessage(EOFPartitionCounter, FlatBuffers);
  }
}

/// Subscribes at an offset on a specified topic and consumes data until
/// manually terminated, or optionally, until reaches end of all partitions
///
/// \param TopicName
/// \param Offset Starting offset, to consume from
/// \param TerminateAtEndOfTopic terminate at end of topic. For unit tests.
void RequestHandler::subscribeAndConsume(const std::string &TopicName,
                                         int64_t Offset,
                                         bool TerminateAtEndOfTopic) {
  if (verifyOffset(Offset, TopicName))
    throw ArgumentException("Offset not valid!");

  int EOFPartitionCounter = 0;
  int NumberOfPartitions =
      KafkaConnection->getNumberOfTopicPartitions(TopicName);

  KafkaConnection->subscribeAtOffset(Offset, TopicName);
  FlatbuffersTranslator FlatBuffers(SchemaPath);
  if (TerminateAtEndOfTopic) {
    while (EOFPartitionCounter < NumberOfPartitions) {
      consumeSingleMessage(EOFPartitionCounter, FlatBuffers);
    }
  } else {
    while (true) {
      consumeSingleMessage(EOFPartitionCounter, FlatBuffers);
    }
  }
}

/// Subscribes to a TopicName and consumes NumberOfMessages from Offset from
/// specified Partition.
///
/// \param TopicName
/// \param NumberOfMessages
/// \param Partition
/// \param Offset
void RequestHandler::subscribeAndConsume(const std::string &TopicName,
                                         const int64_t NumberOfMessages,
                                         const int Partition,
                                         const int64_t Offset) {
  if (verifyOffset(Offset, TopicName))
    throw ArgumentException("Lower offset not valid!");
  if (verifyOffset(Offset + NumberOfMessages, TopicName))
    throw ArgumentException("Cannot show that many messages!");

  int EOFPartitionCounter = 0;
  int NumberOfPartitions =
      KafkaConnection->getNumberOfTopicPartitions(TopicName);

  KafkaConnection->subscribeAtOffset(Offset, TopicName);
  FlatbuffersTranslator FlatBuffers(SchemaPath);
  int MessagesCounter = 0;
  while (EOFPartitionCounter < NumberOfPartitions &&
         MessagesCounter <= NumberOfMessages) {
    if (consumeSingleMessage(EOFPartitionCounter, FlatBuffers))
      MessagesCounter++;
  }
}

bool RequestHandler::consumeSingleMessage(int &EOFPartitionCounter,
                                          FlatbuffersTranslator &FlatBuffers) {
  try {
    KafkaMessageMetadataStruct MessageData;
    MessageData = KafkaConnection->consume();
    MessageData.TimestampISO = timestampToReadable(MessageData.Timestamp);
    printKafkaMessage(MessageData, EOFPartitionCounter, FlatBuffers);
    return true; // got a message
  } catch (TimeoutException &Exception) {
    Logger->debug(Exception.what());
  }
  return false; // didn't get a message
}
