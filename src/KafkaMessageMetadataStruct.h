#pragma once

#include <stdint.h>
#include <string>

struct KafkaMessageMetadataStruct {
  std::int64_t Offset;
  int64_t Timestamp;
  int32_t Partition;
  std::string Payload;
  bool PartitionEOF = false;
  std::string TimestampISO;
  std::string Key;
  bool KeyPresent = false;
};
