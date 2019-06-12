#include "GetSchemaPath.h"
#include <boost/dll.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

/// Update schemas from github repository at runtime if possible, else fall back
/// on using schema included at build time
/// \return Path to schema directory
std::string getSchemasPath() {
  boost::system::error_code Error;
  auto BinDirPath = boost::dll::program_location(Error).parent_path();
  auto BuildDir = BinDirPath.parent_path();
  auto SchemaPath = BuildDir / "schemas";

  return SchemaPath.string();
}
