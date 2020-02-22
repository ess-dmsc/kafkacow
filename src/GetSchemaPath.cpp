#include <iostream>
#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <climits>
#include <unistd.h>     //readlink
#endif

#include "Filesystem.h"
#include "GetSchemaPath.h"

fs::path getExePath()
{
#ifdef _WIN32
    wchar_t Path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, Path, MAX_PATH);
    return Path;
#else
    char Result[PATH_MAX];
    ssize_t Count = readlink("/proc/self/exe", Result, PATH_MAX);
    return std::string(Result, (Count > 0) ? Count : 0);
#endif
}

/// Get path to directory containing the flatbuffer schema files
/// \return Path to schema directory
std::string getSchemaPath() {
    auto const BinDirPath = getExePath().parent_path();
    auto const BuildDir = BinDirPath.parent_path();
    auto const SchemaPath = BuildDir / "schemas";
    return SchemaPath.string();
}
