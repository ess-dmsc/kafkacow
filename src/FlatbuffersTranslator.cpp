#include "FlatbuffersTranslator.h"
#include "ArgumentsException.h"
#include <boost/filesystem.hpp>
#include <iostream>

/// Deserializes Kafka message or, if no schema found, assumes message is in
/// JSON and simply returns it.
/// Returns string with YAML.
///
/// \param Message
std::string FlatbuffersTranslator::deserializeToYAML(
    KafkaMessageMetadataStruct MessageData) {
  // get the ID from a message
  std::string FileID = MessageData.Payload.substr(4, 4);
  if (FileIDMap.find(FileID) ==
      FileIDMap.end()) { // if no ID present in the map:

    std::pair<bool, std::string> SchemaFile = getSchemaPathForID(FileID);

    // if FileID is invalid, assume message is in JSON and return it
    if (!SchemaFile.first)
      return MessageData.Payload;

    std::string Schema;
    bool ok = flatbuffers::LoadFile(SchemaFile.second.c_str(), false, &Schema);
    if (!ok) {
      Logger->error("Couldn't load schema files!\n");
    }

    std::unique_ptr<flatbuffers::Parser> Parser =
        createParser(SchemaFile.second, MessageData.Payload, Schema);

    // save translated message
    std::string YAMLMessage;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &YAMLMessage))
      Logger->error("Couldn't generate new text!\n");

    // put schema path and schema into the map
    FileIDMap.emplace(FileID, std::make_pair(SchemaFile.second, Schema));
    return YAMLMessage;
  } else { // create a parser using schema loaded in the map
    std::unique_ptr<flatbuffers::Parser> Parser = createParser(
        FileIDMap[FileID].first, MessageData.Payload, FileIDMap[FileID].second);
    std::string YAMLMessage;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &YAMLMessage))
      Logger->error("Couldn't generate text using existing parser!\n");
    return YAMLMessage;
  }
}

/// Scans the directory FullPath for a schema containing FileID in its name and
/// returns path to it as a string and flag set to TRUE. If no schema found,
/// returns an empty string and flag FALSE.
///
/// \param FileID
/// \return
std::pair<bool, std::string>
FlatbuffersTranslator::getSchemaPathForID(const std::string &FileID) {
  boost::filesystem::directory_iterator DirectoryIterator(FullPath), e;
  std::vector<boost::filesystem::path> Paths(DirectoryIterator, e);
  for (auto &DirectoryEntry : Paths) {
    if (DirectoryEntry.string().find(FileID) != std::string::npos) {
      // if schema found, return TRUE and path
      return std::make_pair(true, DirectoryEntry.string());
    }
  }
  // if no valid schema for identifier, return FALSE
  return std::make_pair(false, "");
}

/// Creates a Flatbuffers::Parser for a specified schema and returns it.
///
/// \param FullName
/// \param Message
/// \param Schema
/// \return
std::unique_ptr<flatbuffers::Parser>
FlatbuffersTranslator::createParser(const std::string &FullName,
                                    const std::string &Message,
                                    const std::string &Schema) {
  flatbuffers::IDLOptions opts;
  std::unique_ptr<flatbuffers::Parser> Parser(new flatbuffers::Parser(opts));
  Parser->builder_.Clear();
  const char *include_directories[] = {FullPath.c_str(), nullptr};
  bool ok =
      Parser->Parse(Schema.c_str(), include_directories, FullName.c_str());
  if (!ok)
    Logger->error("Couldn't parse the schema!\n");
  Parser->builder_.PushFlatBuffer(
      reinterpret_cast<const uint8_t *>(Message.c_str()), Message.length());
  return Parser;
}
