#pragma once
#include <stdint.h>

struct OffsetsStruct {
  int64_t LowOffset;
  int64_t HighOffset;
  int32_t PartitionId;
};