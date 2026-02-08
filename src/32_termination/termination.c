/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 32 — Process Termination                               ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: exit, _exit, atexit, signals, return codes, core dumps ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — How a Process Ends
 * ════════════════════════════════════════════════════════════════════ */
static void demo_how_process_ends(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — How a Process Ends                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("A process can terminate in several ways:\n\n");

    printf("  Normal termination:\n");
    printf("    1. return from main()     → calls exit(return_value)\n");
    printf("    2. exit(status)           → cleanup + _exit()\n");
    printf("    3. _exit(status) / _Exit()→ immediate kernel exit\n");
    printf("    4. Last thread calls pthread_exit()\n\n");

    printf("  Abnormal termination:\n");
    printf("    5. abort()                → sends SIGABRT\n");
    printf("    6. Signal (SIGSEGV, SIGTERM, SIGKILL, etc.)\n");
    printf("    7. Last thread cancelled  (pthread_cancel)\n\n");

    printf("  ┌──────────────────────────────────────────────────────┐\n");
    printf("  │  return from main()                                  │\n");
    printf("  │       │                                              │\n");
    printf("  │       ▼                                              │\n");
    printf("  │  exit(status)                                        │\n");
    printf("  │       │                                              │\n");
    printf("  │       ├── atexit handlers (LIFO)                     │\n");
    printf("  │       ├── .fini_array destructors                    │\n");
    printf("  │       ├── flush stdio buffers                        │\n");
    printf("  │       ├── close tmpfile() files                      │\n");
    printf("  │       │                                              │\n");
    printf("  │       ▼                                              │\n");
    printf("  │  _exit(status)  ← syscall                            │\n");
    printf("  │       │                                              │\n");
    printf("  │       ├── Close all file descriptors                 │\n");
    printf("  │       ├── Send SIGCHLD to parent                     │\n");
    printf("  │       ├── Reparent children to init (PID 1)          │\n");
    printf("  │       ├── Release memory mappings                    │\n");
    printf("  │       │                                              │\n");
    printf("  │       ▼                                              │\n");
    printf("  │  Zombie state (until parent calls wait())            │\n");
    printf("  └──────────────────────────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — exit() vs _exit() vs _Exit()
 * ════════════════════════════════════════════════════════════════════ */
static void demo_exit_variants(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — exit() vs _exit() vs _Exit()               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌─────────────────┬────────────────────────────────────────┐\n");
    printf("  │ Function        │ Behaviour                              │\n");
    printf("  ├─────────────────┼────────────────────────────────────────┤\n");
    printf("  │ return n;       │ From main(): same as exit(n).          │\n");
    printf("  │                 │ From other func: returns to caller.    │\n");
    printf("  ├─────────────────┼────────────────────────────────────────┤\n");
    printf("  │ exit(n)         │ Calls atexit handlers, flushes stdio,  │\n");
    printf("  │ (stdlib.h)      │ runs .fini_array, then calls _exit(n).│\n");
    printf("  ├─────────────────┼────────────────────────────────────────┤\n");
    printf("  │ _exit(n)        │ Immediate kernel exit. NO atexit,      │\n");
    printf("  │ (unistd.h)      │ NO stdio flush, NO destructors.       │\n");
    printf("  ├─────────────────┼────────────────────────────────────────┤\n");
    printf("  │ _Exit(n)        │ Same as _exit(n). C99 standard.       │\n");
    printf("  │ (stdlib.h)      │                                        │\n");
    printf("  ├─────────────────┼────────────────────────────────────────┤\n");
    printf("  │ abort()         │ Raises SIGABRT. May produce core dump. │\n");
    printf("  │ (stdlib.h)      │ Does NOT call atexit handlers.         │\n");
    printf("  └─────────────────┴────────────────────────────────────────┘\n\n");

    printf("  When to use _exit():\n");
    printf("    - After fork(), in the child process, if exec() fails.\n");
    printf("    - Prevents child from flushing parent's stdio buffers.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Exit Status Codes
 * ════════════════════════════════════════════════════════════════════ */
static void demo_exit_codes(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Exit Status Codes                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  The exit status is an 8-bit value (0-255) passed to the parent.\n\n");

    printf("  Conventions:\n");
    printf("  ┌────────┬──────────────────────────────────────────────┐\n");
    printf("  │ Code   │ Meaning                                      │\n");
    printf("  ├────────┼──────────────────────────────────────────────┤\n");
    printf("  │ 0      │ Success (EXIT_SUCCESS)                       │\n");
    printf("  │ 1      │ General error (EXIT_FAILURE)                 │\n");
    printf("  │ 2      │ Misuse of shell command                      │\n");
    printf("  │ 126    │ Command not executable                       │\n");
    printf("  │ 127    │ Command not found                            │\n");
    printf("  │ 128+N  │ Killed by signal N (e.g., 137 = SIGKILL)    │\n");
    printf("  │ 130    │ Ctrl+C (128 + SIGINT=2)                     │\n");
    printf("  │ 139    │ Segfault (128 + SIGSEGV=11)                 │\n");
    printf("  └────────┴──────────────────────────────────────────────┘\n\n");

    printf("  Shell: echo $?       ← get last command's exit status\n");
    printf("  C:     WEXITSTATUS(status) after wait/waitpid\n\n");

    printf("  EXIT_SUCCESS = %d\n", EXIT_SUCCESS);
    printf("  EXIT_FAILURE = %d\n\n", EXIT_FAILURE);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Signal-Based Termination
 * ════════════════════════════════════════════════════════════════════ */
static void demo_signals(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Signal-Based Termination                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Common fatal signals:\n");
    printf("  ┌───────────┬────────┬─────────────────────────────────┐\n");
    printf("  │ Signal    │ Number │ Cause                           │\n");
    printf("  ├───────────┼────────┼─────────────────────────────────┤\n");
    printf("  │ SIGSEGV   │   11   │ Invalid memory reference        │\n");
    printf("  │ SIGBUS    │    7   │ Bus error (alignment)           │\n");
    printf("  │ SIGFPE    │    8   │ Floating point exception / 0    │\n");
    printf("  │ SIGABRT   │    6   │ abort() called                  │\n");
    printf("  │ SIGTERM   │   15   │ Termination request (kill)      │\n");
    printf("  │ SIGKILL   │    9   │ Forced kill (uncatchable)       │\n");
    printf("  │ SIGINT    │    2   │ Ctrl+C from terminal            │\n");
    printf("  │ SIGPIPE   │   13   │ Broken pipe (no reader)         │\n");
    printf("  └───────────┴────────┴─────────────────────────────────┘\n\n");

    printf("  SIGKILL and SIGSTOP cannot be caught or ignored.\n\n");

    printf("  Default actions:\n");
    printf("    Terminate:       SIGTERM, SIGINT, SIGPIPE\n");
    printf("    Terminate+core:  SIGSEGV, SIGBUS, SIGFPE, SIGABRT\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Demonstrating wait() and Exit Status
 * ════════════════════════════════════════════════════════════════════ */
static void demo_wait_status(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — wait() and Exit Status Demo                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Demo 1: Normal exit */
    pid_t pid1 = fork();
    if (pid1 == 0) {
        _exit(42);  /* child exits with status 42 */
    } else if (pid1 > 0) {
        int status;
        waitpid(pid1, &status, 0);
        if (WIFEXITED(status)) {
            printf("  Child 1: exited normally, status = %d\n",
                   WEXITSTATUS(status));
        }
    }

    /* Demo 2: Signal termination */
    pid_t pid2 = fork();
    if (pid2 == 0) {
        raise(SIGUSR1);  /* child sends itself SIGUSR1 (terminates) */
        _exit(1);  /* not reached */
    } else if (pid2 > 0) {
        int status;
        waitpid(pid2, &status, 0);
        if (WIFSIGNALED(status)) {
            printf("  Child 2: killed by signal %d (%s)\n",
                   WTERMSIG(status),
                   WTERMSIG(status) == SIGUSR1 ? "SIGUSR1" : "other");
        }
    }

    printf("\n  Wait macros:\n");
    printf("    WIFEXITED(status)    — true if normal exit\n");
    printf("    WEXITSTATUS(status)  — get exit code (0-255)\n");
    printf("    WIFSIGNALED(status)  — true if killed by signal\n");
    printf("    WTERMSIG(status)     — get signal number\n");
    printf("    WCOREDUMP(status)    — true if core was dumped\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Zombie and Orphan Processes
 * ════════════════════════════════════════════════════════════════════ */
static void demo_zombie_orphan(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Zombie and Orphan Processes                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Zombie process:\n");
    printf("    A child that has exited but parent hasn't called wait().\n");
    printf("    The kernel keeps the exit status in the process table.\n");
    printf("    Shows as 'Z' in ps output.\n");
    printf("    Cleaned up when parent calls wait() or parent exits.\n\n");

    printf("  Orphan process:\n");
    printf("    Parent exits before child. Child is reparented to init (PID 1).\n");
    printf("    init automatically calls wait() → no permanent zombies.\n\n");

    printf("  Preventing zombies:\n");
    printf("    1. Always wait() for children.\n");
    printf("    2. Use signal(SIGCHLD, SIG_IGN) — auto-reap.\n");
    printf("    3. Handle SIGCHLD with waitpid(-1, ..., WNOHANG).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Core Dumps
 * ════════════════════════════════════════════════════════════════════ */
static void demo_core_dumps(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Core Dumps                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  When a process crashes (SIGSEGV, SIGFPE, SIGABRT),\n");
    printf("  the kernel can write a core dump file.\n\n");

    printf("  Enable core dumps:\n");
    printf("    ulimit -c unlimited\n\n");

    printf("  Core file location:\n");
    printf("    cat /proc/sys/kernel/core_pattern\n");
    printf("    Typical: core, or |/usr/lib/systemd/systemd-coredump\n\n");

    printf("  Analyse with GDB:\n");
    printf("    gdb ./program core\n");
    printf("    (gdb) bt           # see where it crashed\n");
    printf("    (gdb) info registers\n");
    printf("    (gdb) list         # see source at crash point\n\n");

    printf("  The core file contains:\n");
    printf("    - Register state at crash time\n");
    printf("    - All memory mappings (stack, heap, data)\n");
    printf("    - Signal that caused the crash\n");
    printf("    - Thread information\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Kernel Cleanup on exit
 * ════════════════════════════════════════════════════════════════════ */
static void demo_kernel_cleanup(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Kernel Cleanup                              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  When a process exits, the kernel:\n\n");
    printf("    1. Closes all open file descriptors.\n");
    printf("    2. Releases all memory mappings.\n");
    printf("    3. Removes the process from the scheduler.\n");
    printf("    4. Sends SIGCHLD to the parent.\n");
    printf("    5. Reparents children to init.\n");
    printf("    6. Releases SysV semaphore adjustments.\n");
    printf("    7. Releases all locked memory (mlock).\n");
    printf("    8. Decrements shared memory reference counts.\n");
    printf("    9. Keeps a tiny 'zombie' entry until parent reaps it.\n\n");

    printf("  Resources NOT automatically cleaned up:\n");
    printf("    - SysV shared memory segments (shmget/shmat)\n");
    printf("    - POSIX named semaphores (sem_open)\n");
    printf("    - Files created on disk\n");
    printf("    - POSIX message queues\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 32 — Process Termination                           ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_how_process_ends();
    demo_exit_variants();
    demo_exit_codes();
    demo_signals();
    demo_wait_status();
    demo_zombie_orphan();
    demo_core_dumps();
    demo_kernel_cleanup();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 32 — Process Termination\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
