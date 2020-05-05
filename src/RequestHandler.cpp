#include "RequestHandler.h"
#include "CustomExceptions.h"
#include "JSONPrinting.h"
#include <chrono>
#include <flatbuffers/flatbuffers.h>
#include <fmt/format.h>
#include <fstream>

namespace {
/// Prints and formats message Metadata.
///
/// \param MessageData
void printMessageMetadata(Kafka::MessageMetadataStruct &MessageData,
                          const std::string &FileIdentifier) {
  std::cout << fmt::format(
      "\n{:_>93}{}  ||  {}{:>36}\n\nTimestamp: {:>11} || PartitionID: "
      "{:>5} || Offset: {:>7} || File Identifier: {} ||",
      "\n", MessageData.TimestampISO, MessageData.TimestampISO8601, "||",
      MessageData.Timestamp, MessageData.Partition, MessageData.Offset,
      FileIdentifier);
  if (MessageData.KeyPresent) {
    std::cout << fmt::format(" Key: {}", MessageData.Key);
  }
  std::cout << "\n";
}

std::string timestampToReadable(const int64_t &Timestamp) {
  using namespace std;
  time_t Seconds = Timestamp / 1000;
  ctime(&Seconds);
  istringstream iss(asctime(gmtime(&Seconds)));
  vector<string> tokens{istream_iterator<string>{iss},
                        istream_iterator<string>{}};
  return fmt::format("{} {}-{}-{} {}.{}", tokens[0], tokens[2], tokens[1],
                     tokens[4], tokens[3], Timestamp % 1000);
}

std::string timestampToISO8601(const int64_t &Timestamp) {
  char DateString[25];
  time_t Seconds = Timestamp / 1000;
  strftime(DateString, 25, "%FT%T", gmtime(&Seconds));
  return fmt::format("{}.{}", DateString, Timestamp % 1000);
}
}

/// Checks which mode(consumer/metadata/producer) is chosen and
/// calls method responsible for handling one of the modes or throws
/// ArgumentsException if arguments invalid.
/// \param UserArguments
void RequestHandler::checkAndRun() {
  if (UserArguments.ProducerMode) {
    runProducer();
  } else if (UserArguments.ConsumerMode) {
    checkConsumerModeArguments();
  } else {
    checkMetadataModeArguments();
  }
}

/// Analyzes user arguments to determine which consumer mode functionality to
/// run.
///
/// \param UserArguments
/// \param TerminateAtEndOfTopic terminate at end of topic. For unit tests.
void RequestHandler::checkConsumerModeArguments(bool TerminateAtEndOfTopic) {
  if (!UserArguments.JSONPath.empty()) {
    throw ArgumentException("Cannot run consumer mode with -f specified. Did "
                            "you want to use -P mode?");
  }
  if (UserArguments.TopicName.empty()) {
    throw ArgumentException("Please specify topic!");
  }
  checkIfTopicEmpty(UserArguments.TopicName);
  // if date specified and not in conflict with OffsetToStart, parse it and use
  // offset from date.
  if (!UserArguments.ISODate.empty()) {
    if (UserArguments.OffsetToStart > -2) {
      throw ArgumentException("Could not understand arguments: please specify "
                              "either --date or --offset");
    }
    UserArguments.OffsetToStart =
        getOffsetForDate(UserArguments.ISODate, UserArguments.TopicName);
  }

  // consume everything
  if (UserArguments.GoBack == -2 && UserArguments.OffsetToStart == -2) {
    printEntireTopic(UserArguments.TopicName, TerminateAtEndOfTopic);
  } else {
    {
      // consume range
      if (UserArguments.GoBack > -2 && UserArguments.OffsetToStart > -2) {
        subscribeAndConsume(UserArguments.TopicName, UserArguments.GoBack,
                            UserArguments.OffsetToStart);
      }
      // consume from offset/date
      else if (UserArguments.OffsetToStart > -2) {
        subscribeAndConsume(UserArguments.TopicName,
                            UserArguments.OffsetToStart, TerminateAtEndOfTopic);
      } else {
        // consume last N
        if (UserArguments.PartitionToConsume != -1) {
          subscribeAndConsume(UserArguments.TopicName, UserArguments.GoBack,
                              UserArguments.PartitionToConsume,
                              TerminateAtEndOfTopic);
        } else {
          Logger->error("Please specify partition");
        }
      }
    }
  }
}

void RequestHandler::checkIfTopicEmpty(const std::string &TopicName) {
  std::vector<OffsetsStruct> HighAndLowOffsets =
      KafkaConsumer->getTopicsHighAndLowOffsets(TopicName);
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
void RequestHandler::checkMetadataModeArguments() {
  if (!UserArguments.JSONPath.empty()) {
    throw ArgumentException("Cannot run consumer mode with -f specified. Did "
                            "you want to use -P mode?");
  }
  if (UserArguments.ShowAllTopics)
    std::cout << KafkaConsumer->getAllTopics() << "\n";
  else if (!UserArguments.TopicName.empty())
    showTopicPartitionOffsets();
  else if (!UserArguments.ShowAllTopics)
    std::cout << KafkaConsumer->showAllMetadata();
}

void RequestHandler::runProducer() {
  FlatbuffersTranslator FlatBuffers(SchemaPath);

  auto Message = serializeMessage(UserArguments.JSONPath);
  KafkaProducer->produce(std::move(Message));
  Logger->info("Message produced!");
}

/// Ensures there are messages to read at offset provided by the user, otherwise
/// throws an ArgumentsException.
///
/// \param Offset
/// \param TopicName
bool RequestHandler::verifyOffset(const int64_t Offset,
                                  const std::string &TopicName) {
  std::vector<OffsetsStruct> Offsets =
      KafkaConsumer->getTopicsHighAndLowOffsets(TopicName);
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
      KafkaConsumer->getPartitionHighAndLowOffsets(TopicName, Partition);
  if (Struct.HighOffset - Struct.LowOffset < NLast)
    throw ArgumentException("Cannot display that many messages!");
}

/// Prints to screen a list of partitions' IDs and their low/high offsets.
///
/// \param UserArguments
void RequestHandler::showTopicPartitionOffsets() {
  std::cout << UserArguments.TopicName << "\n";
  for (auto &SingleStruct :
       KafkaConsumer->getTopicsHighAndLowOffsets(UserArguments.TopicName)) {
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
void RequestHandler::printKafkaMessage(
    Kafka::MessageMetadataStruct &MessageData, int &EOFPartitionCounter,
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
              "{}", getTruncatedMessage(JSONMessage,
                UserArguments.ShowCompactMessage
                ? 1
                : UserArguments.Indentation,
                UserArguments.ShowCompactMessage))
                << std::endl;
  }
  if (MessageData.PartitionEOF)
    EOFPartitionCounter++;
}

/// Calculates topic's lowest offset and subscribes to it to print the entire
/// topic.
///
/// \param TopicName
/// \param TerminateAtEndOfTopic terminate at end of topic. For unit tests.
void RequestHandler::printEntireTopic(const std::string &TopicName,
                                      bool TerminateAtEndOfTopic) {
  std::vector<OffsetsStruct> OffsetsStruct =
      KafkaConsumer->getTopicsHighAndLowOffsets(TopicName);
  int64_t MinOffset = OffsetsStruct[0].LowOffset;
  for (auto OffsetStruct : OffsetsStruct) {
    if (OffsetStruct.LowOffset < MinOffset)
      MinOffset = OffsetStruct.LowOffset;
  }
  subscribeAndConsume(TopicName, MinOffset, TerminateAtEndOfTopic);
}

/// Subscribes to NumberOfMessages from Partition of specified TopicName and
/// consumes the data.
///
/// \param TopicName
/// \param NumberOfMessages
/// \param Partition
void RequestHandler::subscribeAndConsume(const std::string &TopicName,
                                         int64_t NumberOfMessages,
                                         int Partition,
                                         bool TerminateAtEndOfTopic) {
  verifyNLast(NumberOfMessages, TopicName, Partition);
  KafkaConsumer->subscribeToLastNMessages(NumberOfMessages, TopicName,
                                          Partition);
  consumeAllSubscribed(TopicName, TerminateAtEndOfTopic);
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
  KafkaConsumer->subscribeAtOffset(Offset, TopicName);
  consumeAllSubscribed(TopicName, TerminateAtEndOfTopic);
}

/// Subscribes to a TopicName and consumes NumberOfMessages from Offset from
/// specified Partition.
///
/// \param TopicName
/// \param NumberOfMessages
/// \param Offset
void RequestHandler::subscribeAndConsume(const std::string &TopicName,
                                         const int64_t NumberOfMessages,
                                         const int64_t Offset) {
  if (verifyOffset(Offset, TopicName))
    throw ArgumentException("Lower offset not valid!");
  if (verifyOffset(Offset + NumberOfMessages, TopicName))
    throw ArgumentException("Cannot show that many messages!");

  KafkaConsumer->subscribeAtOffset(Offset, TopicName);
  consumeNSubscribed(TopicName, NumberOfMessages);
}

void RequestHandler::consumeNSubscribed(const std::string &Topic,
                                        int64_t NumberOfMessages) {
  int EOFPartitionCounter = 0;
  int NumberOfPartitions = KafkaConsumer->getNumberOfTopicPartitions(Topic);
  FlatbuffersTranslator FlatBuffers(SchemaPath);
  int MessagesCounter = 0;
  while (EOFPartitionCounter < NumberOfPartitions &&
         MessagesCounter <= NumberOfMessages - 1) {
    if (consumeSingleMessage(EOFPartitionCounter, FlatBuffers))
      MessagesCounter++;
  }
}

bool RequestHandler::consumeSingleMessage(int &EOFPartitionCounter,
                                          FlatbuffersTranslator &FlatBuffers) {
  try {
    Kafka::MessageMetadataStruct MessageData;
    MessageData = KafkaConsumer->consume();
    MessageData.TimestampISO = timestampToReadable(MessageData.Timestamp);
    MessageData.TimestampISO8601 = timestampToISO8601(MessageData.Timestamp);
    printKafkaMessage(MessageData, EOFPartitionCounter, FlatBuffers);
    return true; // got a message
  } catch (TimeoutException &Exception) {
    Logger->debug(Exception.what());
  }
  return false; // didn't get a message
}

void RequestHandler::consumeAllSubscribed(const std::string &Topic,
                                          bool TerminateAtEndOfTopic) {
  int EOFPartitionCounter = 0;
  FlatbuffersTranslator FlatBuffers(SchemaPath);
  int NumberOfPartitions = KafkaConsumer->getNumberOfTopicPartitions(Topic);
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

int64_t RequestHandler::getOffsetForDate(const std::string &Date,
                                         const std::string &Topic) {
  return KafkaConsumer->getOffsetForDate(Date, Topic);
}
