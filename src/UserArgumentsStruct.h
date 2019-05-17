#pragma once

#include <string>

struct UserArgumentStruct {
  std::string Name;

  // value -2 signalises that the variable hasn't been set by a user and won't
  // be used
  std::int16_t GoBack = -2;
  std::int64_t OffsetToStart = -2;

  int PartitionToConsume = -1;
  int Indentation = 4;

  bool ShowAllTopics = false;
  bool ConsumerMode = false;
  bool MetadataMode = false;
  bool ProducerMode = false;
  bool ShowEntireMessage = false;
};
