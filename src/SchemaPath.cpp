#include "SchemaPath.h"
#include <boost/filesystem.hpp>

std::string getSchemaPath(const std::string &BinPath) {
  namespace fs = boost::filesystem;

  fs::path FullBinPath(fs::initial_path<fs::path>());
  FullBinPath = fs::system_complete(fs::path(BinPath));
  auto BinDirPath = FullBinPath.parent_path();
  auto BuildDir = BinDirPath.parent_path();
  auto SchemaPath = BuildDir / "schemas";

  return SchemaPath.string();
}
