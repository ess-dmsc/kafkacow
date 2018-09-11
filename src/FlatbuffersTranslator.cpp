#include "FlatbuffersTranslator.h"
#include "ArgumentsException.h"
#include <boost/filesystem.hpp>
#include <iostream>

void FlatbuffersTranslator::getFileID(KafkaMessageMetadataStruct MessageData) {
  // get the ID from a message
  std::string FileID = MessageData.PayloadToReturn.substr(4, 4);
  if (FileIDMap.find(FileID) ==
      FileIDMap.end()) { // if no ID present in the map:

    // get schema name and path for FileID
    std::string SchemaFile = getSchemaPathForID(FileID);
    std::string Schema;
    bool ok = flatbuffers::LoadFile(SchemaFile.c_str(), false, &Schema);
    if (!ok) {
      // Logger->error("Couldn't load schema files!\n");
    }

    std::unique_ptr<flatbuffers::Parser> Parser =
        createParser(SchemaFile, MessageData.PayloadToReturn, Schema);
    std::string JSONMessage;

    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &JSONMessage))
      Logger->error("Couldn't generate new text!\n");

    // put schema path and schema into the map
    FileIDMap.emplace(FileID, std::make_pair(SchemaFile, Schema));
    printMessage(JSONMessage, MessageData);
  } else { // create a parser using schema loaded in the map
    std::unique_ptr<flatbuffers::Parser> Parser =
        createParser(FileIDMap[FileID].first, MessageData.PayloadToReturn,
                     FileIDMap[FileID].second);
    std::string JSONMessage;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &JSONMessage))
      Logger->error("Couldn't generate text using existing parser!\n");
    printMessage(JSONMessage, MessageData);
  }
}

// find a schema file name relevant to the ID and return it
std::string
FlatbuffersTranslator::getSchemaPathForID(const std::string &FileID) {
  boost::filesystem::directory_iterator DirectoryIterator(FullPath), e;
  std::vector<boost::filesystem::path> Paths(DirectoryIterator, e);
  for (auto &DirectoryEntry : Paths) {
    if (DirectoryEntry.string().find(FileID) != std::string::npos) {
      return DirectoryEntry.string();
    }
  }
  throw ArgumentsException("No relevant schema!");
}

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

void FlatbuffersTranslator::printMessage(
    const std::string &JSONMessage, KafkaMessageMetadataStruct MessageData) {
  std::cout << "Partition: " << MessageData.Partition
            << " || Offset: " << MessageData.Offset
            << " || Timestamp: " << MessageData.Timestamp << "\n";
  std::cout << JSONMessage;
}
