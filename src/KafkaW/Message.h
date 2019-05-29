#pragma once

#include <cstdlib>
#include <flatbuffers/flatbuffers.h>

namespace KafkaW {

class Message {
public:
  explicit Message(flatbuffers::DetachedBuffer InputBuffer)
      : Buffer(std::move(InputBuffer)) {}

  char *data() { return reinterpret_cast<char *>(Buffer.data()); }
  size_t size() { return Buffer.size(); }

private:
  flatbuffers::DetachedBuffer Buffer;
};
}
