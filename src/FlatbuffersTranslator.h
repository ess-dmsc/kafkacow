#pragma once

#include <flatbuffers/idl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

class FlatbuffersTranslator {
private:
  std::map<std::string, flatbuffers::Parser> CodesMap;

public:
  FlatbuffersTranslator() {}

  void TakeCode(std::string Message);

  void LoadFile();
};