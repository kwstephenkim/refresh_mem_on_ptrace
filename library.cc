#include "library.h"

#include <libgen.h>
#include <unistd.h>

#include <cassert>
#include <climits>
#include <cstring>

extern const char kDebuggee[] = "debuggee.x";

std::string GetExecPathDir() {
  char exec_path[ PATH_MAX ] = {0};
  ssize_t count = readlink( "/proc/self/exe", exec_path, PATH_MAX );
  assert("Readlink failed" && (count > 0));
  auto* dir_name = dirname(exec_path);
  assert("dirname returns nullptr" && dir_name);
  return std::string(dir_name);
}

char** CloneArgv(int argc, char* argv[], const std::string& prog_path) {
  char** new_argv = new char*[argc + 1];
  new_argv[argc] = nullptr;
  for (int i = 1; i < argc; i++) {
    auto len = strnlen(argv[i], PATH_MAX);
    new_argv[i] = new char[len + 1];
    new_argv[i][len] = 0;
    strncpy(new_argv[i], argv[i], len);
  }
  auto len = strnlen(prog_path.data(), PATH_MAX);
  new_argv[0] = new char[len + 1];
  strncpy(new_argv[0], prog_path.data(), len);
  return new_argv;
}
