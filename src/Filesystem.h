#pragma once

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
std::cout << "Using std::filesystem\n";
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
