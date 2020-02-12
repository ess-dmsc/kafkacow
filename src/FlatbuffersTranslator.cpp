#include "FlatbuffersTranslator.h"
#include "CustomExceptions.h"
#include <boost/filesystem.hpp>
#include <flatbuffers/flatbuffers.h>
#include <json_json_generated.h>

namespace {
/// Reads JSONPath file and returns message to serialize.
/// \param JSONPath Path to file
/// \return contents of file
std::string getMessageFromFile(const std::string &JSONPath) {
  std::ifstream IfStream(JSONPath);
  std::stringstream StringStream;
  StringStream << IfStream.rdbuf();
  return StringStream.str();
}
}

/// Serializes message found in file JSONPath.
/// \param JSONPath
/// \return Kafka::Message
Kafka::Message serializeMessage(const std::string &JSONPath) {
  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();
  auto FBOffset =
      CreateJsonDataDirect(builder, getMessageFromFile(JSONPath).c_str());
  FinishJsonDataBuffer(builder, FBOffset);
  return Kafka::Message(builder.Release());
}

/// If schema is found, deserializes message and returns it as string.
/// Otherwise assumes message is valid JSON and returns it.
///
/// \param MessageData
/// \param FileID : return value
/// \return single string with JSON message.
std::string FlatbuffersTranslator::deserializeToJSON(
    const Kafka::MessageMetadataStruct &MessageData, std::string &FileID) {
  // get the ID from a message
  if (MessageData.Payload.size() > 8) {
    FileID = MessageData.Payload.substr(4, 4);
  } else {
    FileID.clear();
  }

  if (FileID == "json") {
    return GetJsonData(MessageData.Payload.c_str())->json()->str();
  }

  if (FileIDMap.find(FileID) ==
      FileIDMap.end()) { // if no ID present in the map:

    std::pair<bool, std::string> SchemaFile = getSchemaPathForID(FileID);

    // if FileID is invalid, assume message is in JSON and return it
    if (!SchemaFile.first) {
      FileID = FileID + " (not recognised)";
      return MessageData.Payload;
    }

    std::string Schema;
    bool ok = flatbuffers::LoadFile(SchemaFile.second.c_str(), false, &Schema);
    if (!ok) {
      Logger->error("Couldn't load schema files!\n");
    }

    std::unique_ptr<flatbuffers::Parser> Parser =
        createParser(SchemaFile.second, MessageData.Payload, Schema);

    // save translated message
    std::string DeserializedMessage;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &DeserializedMessage))
      Logger->error("Couldn't generate new text!\n");

    // put schema path and schema into the map
    FileIDMap.emplace(FileID, std::make_pair(SchemaFile.second, Schema));
    auto before = DeserializedMessage;

    return DeserializedMessage;
  } else { // create a parser using schema loaded in the map
    std::unique_ptr<flatbuffers::Parser> Parser = createParser(
        FileIDMap[FileID].first, MessageData.Payload, FileIDMap[FileID].second);
    std::string DeserializedMessage;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &DeserializedMessage))
      Logger->error("Couldn't generate text using existing parser!\n");
    return DeserializedMessage;
  }
}

/// Scans the directory FullPath for a schema containing FileID in its name.
///
/// \param FileID
/// \return std::pair<bool, std::string> with path as string and bool TRUE if
/// path was found, or FALSE and empty string if otherwise.
std::pair<bool, std::string>
FlatbuffersTranslator::getSchemaPathForID(const std::string &FileID) {
  if (FileID.empty())
    return std::make_pair(false, "");

  boost::filesystem::directory_iterator DirectoryIterator(SchemaPath), e;
  std::vector<boost::filesystem::path> Paths(DirectoryIterator, e);
  for (auto &DirectoryEntry : Paths) {
    if (DirectoryEntry.filename().string().find(FileID) != std::string::npos) {
      // if schema found, return TRUE and path
      return std::make_pair(true, DirectoryEntry.string());
    }
  }
  // if no valid schema for identifier, return FALSE
  return std::make_pair(false, "");
}

/// Creates a Flatbuffers::Parser for a specified schema.
///
/// \param FullName name of schema file
/// \param Message  Message to deserialize
/// \param Schema   schema file
/// \return std::unique_ptr<flatbuffers::Parser> to created parser.
std::unique_ptr<flatbuffers::Parser>
FlatbuffersTranslator::createParser(const std::string &FullName,
                                    const std::string &Message,
                                    const std::string &Schema) {
  flatbuffers::IDLOptions opts;

  // Make sure flatbuffers returns strict json. Otherwise messages give parsing
  // errors and break truncation.
  opts.strict_json = true;
  auto Parser = std::make_unique<flatbuffers::Parser>(opts);
  Parser->builder_.Clear();
  const char *include_directories[] = {SchemaPath.c_str(), nullptr};
  bool ok =
      Parser->Parse(Schema.c_str(), include_directories, FullName.c_str());
  if (!ok)
    Logger->error("Couldn't parse the schema!\n");
  Parser->builder_.PushFlatBuffer(
      reinterpret_cast<const uint8_t *>(Message.c_str()), Message.length());
  return Parser;
}
