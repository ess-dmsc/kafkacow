#include "FlatbuffersTranslator.h"
#include "ArgumentsException.h"
#include <boost/filesystem.hpp>
#include <flatbuffers/idl.h>
#include <iostream>

void FlatbuffersTranslator::TakeFileID(std::string Message) {
  // get the ID from a message
  std::string FileID = Message.substr(4, 4);

  if (FileIDMap.find(FileID) ==
      FileIDMap.end()) { // if no ID present in the map:

    std::string Schema;
    std::string contents;

    // get schema name and path for FileID
    std::string SchemaFile = GetSchemaPathForID(FileID);

    bool ok = flatbuffers::LoadFile(SchemaFile.c_str(), false, &Schema);
    if (!ok) {
      std::cerr << ("Couldn't load schema files!\n");
    }

    std::unique_ptr<flatbuffers::Parser> Parser =
        CreateParser(SchemaFile, Message, Schema);
    std::string JSONMessage;

    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(),
                      &JSONMessage))
      std::cerr << ("Couldn't generate new text!\n");
    // print readable message
    std::cout << JSONMessage;
    FileIDMap.emplace(FileID, std::move(Parser));
  } else { // retrieve message using a parser that has already been put to the
           // map
    std::string JSONMessage;

    if (!GenerateText(*FileIDMap[FileID],
                      FileIDMap[FileID]->builder_.GetBufferPointer(),
                      &JSONMessage))
      std::cerr << ("Couldn't generate text using existing parser!\n");
    // print readable message
    PrintMessage(JSONMessage);
  }
}

// find a schema file name relevant to the ID and return it
std::string FlatbuffersTranslator::GetSchemaPathForID(std::string FileID) {
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
FlatbuffersTranslator::CreateParser(std::string FullName, std::string Message,
                                    std::string Schema) {
  flatbuffers::IDLOptions opts;
  std::unique_ptr<flatbuffers::Parser> Parser(new flatbuffers::Parser(opts));
  Parser->builder_.Clear();
  const char *include_directories[] = {FullPath.c_str(), nullptr};
  bool ok =
      Parser->Parse(Schema.c_str(), include_directories, FullName.c_str());
  if (!ok)
    std::cerr << "Couldn't parse the schema!\n";
  Parser->builder_.PushFlatBuffer(
      reinterpret_cast<const uint8_t *>(Message.c_str()), Message.length());
  return Parser;
}

void FlatbuffersTranslator::PrintMessage(std::string JSONMessage) {
  std::cout << JSONMessage;
}
