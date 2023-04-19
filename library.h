#pragma once

#include <string>

std::string GetExecPathDir();

char** CloneArgv(
  int argc, char* argv[], const std::string& prog_path);

extern const char kDebuggee[];
