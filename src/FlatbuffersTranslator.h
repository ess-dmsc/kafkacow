#pragma once

#include <flatbuffers/idl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

class FlatbuffersTranslator {
private:
  std::map<std::string, std::unique_ptr<flatbuffers::Parser>> FileIDMap;
  std::string FullPath = "../schemas/";

public:
  void TakeFileID(std::string Message);

  std::string GetSchemaPathForID(std::string FileID);

  std::unique_ptr<flatbuffers::Parser>
  CreateParser(std::string FullName, std::string Message, std::string Schema);

  void PrintMessage(std::string JSONMessage);
};
