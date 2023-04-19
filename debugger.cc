#include <errno.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include "library.h"

extern char** environ;

static void FirstWaitForDebuggee();
static void SetBreakpoint(const pid_t pid, const long long address);
static void ContinueDebuggee(const pid_t pid);

int main(int argc, char* argv[])
{
  auto pwd = GetExecPathDir();
  auto pid = fork();
  if (!pid) {
    std::string prog_path(pwd);
    prog_path.append("/").append(std::string(kDebuggee));
    std::cout << "Exec... " << prog_path << std::endl;
    // child process, or debuggee
    char** new_argv = CloneArgv(argc, argv, prog_path);

    // PTRACE_TRACEME
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    execvpe(prog_path.c_str(), argv, environ);
    std::cerr << "Failed to execvpe with errno : "
              << strerror(errno);
    assert("Must not reach here, execvpe failed." && false);
  }
  // root process
  FirstWaitForDebuggee();

  // The debuggee is loaded. Now, we can set a breakpoint
  long long address =
    (argv && argv[1] ? strtoll(argv[1], nullptr, 16) : 0x0000000000401134);
  SetBreakpoint(pid, address);
  ContinueDebuggee(pid);

  int wstatus = 0;
  pid_t waited_pid = wait(&wstatus);
  if (!WIFEXITED(wstatus)) {
    std::cerr << "Second wait() must return with exited." << std::endl;
    exit(1);
  }
  // debugee stopped.
  // set breakpoint at main
  return 0;
}

void FirstWaitForDebuggee() {
  int wstatus = 0;
  pid_t waited_pid = wait(&wstatus);
  if (!WIFSTOPPED(wstatus)) {
    std::cerr << "First wait() must return with stopped." << std::endl;
    kill(waited_pid, SIGKILL);
  }
  return;
}

void ContinueDebuggee(const pid_t pid) {
  std::cout << "Continueing the debuggee.." << std::endl;
  if (ptrace(PTRACE_CONT, pid, nullptr, nullptr) < 0) {
    std::cerr << strerror(errno) << std::endl;
    kill(pid, SIGKILL);
  }
}

void SetBreakpoint(const pid_t pid, const long long address) {
  auto old_instruction =
    ptrace(PTRACE_PEEKTEXT, pid, (void*) address, nullptr);
  std::cout << "Read 4 bytes from debuggee's text: " << std::endl
            << "  0x" << std::hex << old_instruction << std::endl;
  return;
}
