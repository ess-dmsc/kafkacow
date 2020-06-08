#pragma once

#include <map>
#include <string>

struct UserArgumentStruct {
  std::string TopicName;
  std::string Broker;
  std::string JSONPath;

  // value -2 signalises that the variable hasn't been set by a user and won't
  // be used
  std::int16_t GoBack = -2;
  std::int64_t OffsetToStart = -2;

  int PartitionToConsume = -1;
  int Indentation = 2;

  std::map<std::string, std::string> KafkaConfiguration = {
      {"metadata.request.timeout.ms", "2000"},
      {"socket.timeout.ms", "10000"},
      {"message.max.bytes", "100000000"},
      {"fetch.message.max.bytes", "100000000"},
      {"fetch.max.bytes", "100000000"},
      {"receive.message.max.bytes",
       "100000512"}, // must be at least fetch.max.bytes + 512
      {"queue.buffering.max.ms", "50"},
      {"api.version.request", "true"},
      {"enable.auto.commit", "false"},
      {"enable.auto.offset.store", "false"},
      {"auto.offset.reset", "largest"},
      {"session.timeout.ms", "10000"}};

  std::string ISODate;
  bool ShowAllTopics = false;
  bool ConsumerMode = false;
  bool MetadataMode = false;
  bool ProducerMode = false;
  bool ShowEntireMessage = false;
  bool ShowCompactMessage = false;
};
