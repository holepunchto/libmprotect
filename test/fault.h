#ifndef FAULT_H
#define FAULT_H

#include <assert.h>

#ifdef _WIN32

#define expect_fault(fn) return 77

#else

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#define expect_fault(fn) \
  do { \
    pid_t pid = fork(); \
    assert(pid >= 0); \
    if (pid == 0) { \
      fn(); \
      _exit(0); \
    } \
    int status; \
    assert(waitpid(pid, &status, 0) == pid); \
    assert(WIFSIGNALED(status)); \
    assert(WTERMSIG(status) == SIGSEGV || WTERMSIG(status) == SIGBUS); \
  } while (0)

#endif

#endif // FAULT_H
