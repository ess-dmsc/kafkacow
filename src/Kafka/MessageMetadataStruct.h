#pragma once

#include <stdint.h>
#include <string>

namespace Kafka {

struct MessageMetadataStruct {
  std::int64_t Offset;
  int64_t Timestamp;
  int32_t Partition;
  std::string Payload;
  bool PartitionEOF = false;
  std::string TimestampISO;
  std::string TimestampISO8601;
  std::string Key;
  bool KeyPresent = false;
};
}
