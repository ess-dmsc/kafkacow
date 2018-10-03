#pragma once

#include <stdint.h>
#include <string>

struct KafkaMessageMetadataStruct {
  std::int32_t Offset;
  int64_t Timestamp;
  int32_t Partition;
  std::string Payload;
  bool PartitionEOF = false;
};
