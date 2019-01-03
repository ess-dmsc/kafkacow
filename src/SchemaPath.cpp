#include "SchemaPath.h"
#include <boost/dll.hpp>
#include <boost/filesystem.hpp>

std::string getSchemaPath() {
  namespace fs = boost::filesystem;

  boost::system::error_code Error;
  auto BinDirPath = boost::dll::program_location(Error).parent_path();
  auto BuildDir = BinDirPath.parent_path();
  auto SchemaPath = BuildDir / "schemas";

  return SchemaPath.string();
}
