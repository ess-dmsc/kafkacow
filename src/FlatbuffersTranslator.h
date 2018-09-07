#pragma once

#include <flatbuffers/idl.h>

class FlatbuffersTranslator {
private:
  std::map<std::string, std::unique_ptr<flatbuffers::Parser>> FileIDMap;
  std::string FullPath = "../schemas/";

public:
  void getFileID(std::string *Message);

  std::string getSchemaPathForID(const std::string &FileID);

  std::unique_ptr<flatbuffers::Parser> createParser(const std::string &FullName,
                                                    const std::string &Message,
                                                    const std::string &Schema);

  void printMessage(const std::string &JSONMessage);
};
