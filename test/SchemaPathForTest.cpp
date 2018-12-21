#include "SchemaPathForTest.h"
#include "../src/SchemaPath.h"

extern std::string BinDirectory;

std::string getSchemaPathForTest() { return getSchemaPath(BinDirectory); }
