#pragma once
#include <stdint.h>
#include <string>
#include <vector>
struct TopicMetadataStruct {
  std::string Name;
  std::vector<int32_t> Partitions;
};