#include "FlatbuffersTranslator.h"
#include <flatbuffers/idl.h>
#include <iostream>

void FlatbuffersTranslator::LoadFile() {

  // Path.append(Code);
  // Path.append(".fbs");
  // std::ifstream Schema(Path,std::ios::binary);
}

void FlatbuffersTranslator::TakeCode(std::string Message) {

  // TODO browse through schema list to find a name that includes Message

  std::string Code = "ev42_events";
  std::string FullPath = "../schemas/";
  FullPath.append(Code);
  FullPath.append(".fbs");
  // if no code present in the map:
  if (CodesMap.find(Code) == CodesMap.end()) {

    std::string Schema;
    std::string contents;

    bool ok = flatbuffers::LoadFile(FullPath.c_str(), false, &Schema);
    if (!ok) {
      std::cerr << ("Couldn't load files!\n");
    }

    flatbuffers::IDLOptions opts;
    std::unique_ptr<flatbuffers::Parser> Parser(new flatbuffers::Parser(opts));
    Parser->builder_.Clear();
    const char *include_directories[] = {"../schemas/", nullptr};
    std::string FullName = Code;
    FullName.append(".fbs");
    ok = Parser->Parse(Schema.c_str(), include_directories, FullName.c_str());
    if (!ok)
      std::cerr << "Couldn't parse the schema!\n";
    Parser->builder_.PushFlatBuffer(
        reinterpret_cast<const uint8_t *>(Message.c_str()), Message.length());
    std::cout << Parser->file_identifier_ << std::endl;
    std::string jsongen;
    if (!GenerateText(*Parser, Parser->builder_.GetBufferPointer(), &jsongen))
      std::cerr << ("Couldn't generate text!\n");
    // print readable message
    std::cout << jsongen;
    // TODO add to Map

  }
  // if code has already been used:
  else {
    // TODO retrieve message using a parser that has already been used
  }
}
