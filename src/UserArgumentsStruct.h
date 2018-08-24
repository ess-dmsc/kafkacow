#pragma once

struct UserArgumentStruct {
  std::string Name;
  std::int16_t GoBack = -2;
  std::int64_t OffsetToStart = -2;
  bool ShowAllTopics;
  bool ConsumerMode;
  bool MetadataMode;
  bool ShowPartitionsOffsets;
};
