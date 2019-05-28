#pragma once

#include "KafkaW/KafkaMessageMetadataStruct.h"
#include "KafkaW/Message.h"
#include <flatbuffers/idl.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

class FlatbuffersTranslator {
public:
  explicit FlatbuffersTranslator(std::string FullSchemaPath)
      : SchemaPath(std::move(FullSchemaPath)), Logger(spdlog::get("LOG")) {}

  std::string deserializeToJSON(KafkaMessageMetadataStruct MessageData,
                                std::string &FileID);

  std::pair<bool, std::string> getSchemaPathForID(const std::string &FileID);

  std::unique_ptr<flatbuffers::Parser> createParser(const std::string &FullName,
                                                    const std::string &Message,
                                                    const std::string &Schema);

  KafkaW::Message serializeMessage(const std::string JSONPath);

private:
  // for each FILEID store path to schema file and schema itself
  std::string getMessageFromFile(const std::string JSONPath);
  std::map<std::string, std::pair<std::string, std::string>> FileIDMap;
  const std::string SchemaPath;
  std::shared_ptr<spdlog::logger> Logger;
};
