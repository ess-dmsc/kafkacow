#include "UpdateSchemas.h"
#include <boost/dll.hpp>
#include <boost/filesystem/path.hpp>

/// Get path to directory containing the flatbuffer schema files
/// \return Path to schema directory
std::string getSchemaPath() {
  boost::system::error_code Error;
  auto BinDirPath = boost::dll::program_location(Error).parent_path();
  auto BuildDir = BinDirPath.parent_path();
  auto SchemaPath = BuildDir / "schemas";
  return SchemaPath.string();
}
