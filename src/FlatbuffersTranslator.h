#pragma once

#include "Kafka/Message.h"
#include "Kafka/MessageMetadataStruct.h"
#include <flatbuffers/idl.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

Kafka::Message serializeMessage(const std::string &JSONPath);

class FlatbuffersTranslator {
public:
  explicit FlatbuffersTranslator(std::string FullSchemaPath)
      : SchemaPath(std::move(FullSchemaPath)), Logger(spdlog::get("LOG")) {}

  std::string deserializeToJSON(const Kafka::MessageMetadataStruct& MessageData,
                                std::string &FileID);

  std::pair<bool, std::string> getSchemaPathForID(const std::string &FileID);

  std::unique_ptr<flatbuffers::Parser> createParser(const std::string &FullName,
                                                    const std::string &Message,
                                                    const std::string &Schema);
private:
  // for each FILEID store path to schema file and schema itself
  std::map<std::string, std::pair<std::string, std::string>> FileIDMap;
  const std::string SchemaPath;
  std::shared_ptr<spdlog::logger> Logger;
};
