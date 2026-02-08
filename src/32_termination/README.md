# Chapter 32 — Program Termination

## Overview
A C program can end in many ways — a clean `return 0` from `main()`, an explicit `exit()`, an abrupt `_exit()`, a fatal signal, or an `abort()`. Each path triggers different cleanup behaviour: `atexit` handlers, stdio flushing, signal delivery, core dump generation, and kernel-level resource reclamation. This chapter covers every termination path, demonstrates `fork()`/`wait()` to observe exit status from the parent side, and explains zombie processes and how the kernel cleans up after a dead process.

## Key Concepts
- `exit()` vs `_exit()` vs `abort()` — what each one does and does not clean up
- Exit codes: convention (0 = success, 1–125 = error, 126–127 = shell, 128+N = signal)
- Fatal signals: `SIGSEGV`, `SIGBUS`, `SIGABRT`, `SIGFPE`, `SIGKILL`, `SIGTERM`
- Wait status macros: `WIFEXITED()`, `WEXITSTATUS()`, `WIFSIGNALED()`, `WTERMSIG()`
- Zombie processes: what they are and how `wait()`/`SIGCHLD` prevents them
- Core dumps: generation, `ulimit -c`, and analysis with GDB
- Kernel cleanup: closing file descriptors, unmapping memory, releasing locks
- The `__attribute__((destructor))` and `atexit()` ordering during `exit()`

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | exit() — Clean Shutdown | Runs atexit handlers, flushes stdio, calls `_exit()` |
| 2 | _exit() — Immediate Exit | Kernel-level exit with no userspace cleanup |
| 3 | abort() and Signals | Raises `SIGABRT`, may produce a core dump |
| 4 | Exit Codes & Conventions | Standard meanings of exit codes 0–255 |
| 5 | Wait Status Macros | Live fork/wait demo: inspecting child termination from the parent |
| 6 | Zombies & Orphans | What happens when a parent doesn't `wait()` |
| 7 | Core Dumps & Kernel Cleanup | Enabling core dumps and post-mortem debugging |

## Building & Running
```bash
make bin/32_termination
./bin/32_termination
```

## Diagrams
- [Concept Diagram](32_termination_concept.png)
- [Code Flow Diagram](32_termination_flow.png)

## Try It Yourself
```bash
# Run the demo and check its exit code
./bin/32_termination
echo "Exit status: $?"

# Time the program execution and resource usage
/usr/bin/time -v ./bin/32_termination

# Enable core dumps for the current shell
ulimit -c unlimited

# Trigger a segfault and examine the core dump
# (if the demo includes a --crash flag)
./bin/32_termination --crash
gdb ./bin/32_termination core

# Observe zombie processes (run in background, don't wait)
./bin/32_termination &
ps aux | grep defunct

# Trace exit-related system calls
strace -e trace=exit,exit_group,wait4,kill ./bin/32_termination

# Check signal disposition
kill -l
```

## Further Reading
- [man 3 exit](https://man7.org/linux/man-pages/man3/exit.3.html) — process termination
- [man 2 wait](https://man7.org/linux/man-pages/man2/wait.2.html) — wait status macros
- [man 7 signal](https://man7.org/linux/man-pages/man7/signal.7.html) — signal overview and default actions
