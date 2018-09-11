#pragma once
#include <stdint.h>
#include <string>

struct KafkaMessageMetadataStruct{
  std::string PayloadToReturn;
  std::int64_t Offset;
  int32_t Partition;
  int64_t Timestamp;
  bool PartitionEOF;
};