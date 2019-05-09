#pragma once

#include <string>

/// Update schemas from github if possible, return path to schema directory
std::string updateSchemas(bool PullUpdates=true);
