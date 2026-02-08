/*
 * Chapter 15 — System Programming
 *
 * System programming means talking directly to the operating system:
 * processes, signals, environment, file descriptors, pipes, and time.
 * These are the POSIX building blocks that every Unix tool is built from.
 *
 * Topics covered:
 *   1. Environment variables (getenv, setenv, defensive coding)
 *   2. Signal handling (async-signal-safe restrictions)
 *   3. Process information (PID, UID, CWD)
 *   4. Fork and exec (COW, file descriptor inheritance)
 *   5. system() — convenience vs security trade-offs
 *   6. Pipes — IPC between parent and child via pipe() + fork()
 *   7. Time functions (time, localtime, strftime)
 *
 * Build: make 15_system
 * Run:   ./bin/15_system
 *
 * Try these:
 *   strace ./bin/15_system 2>&1 | grep -E 'clone|fork|pipe|exec'
 *   MY_VAR=custom ./bin/15_system     # set env var before running
 *   ltrace ./bin/15_system 2>&1 | head -40   # library call trace
 */

#include "../../include/common.h"
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Environment Variables
 * ════════════════════════════════════════════════════════════════ */

static void demo_environment(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Environment Variables                   ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* getenv returns NULL if the variable is not set.
     * ALWAYS check for NULL before using the result — dereferencing
     * NULL is undefined behaviour and will crash your program. */
    const char *home = getenv("HOME");
    const char *path = getenv("PATH");
    const char *missing = getenv("UNLIKELY_VAR_XYZ_12345");

    printf("  HOME = %s\n", home ? home : "(not set)");
    printf("  PATH = %.60s...\n", path ? path : "(not set)");
    printf("  UNLIKELY_VAR = %s\n\n", missing ? missing : "(not set — NULL returned)");

    /* setenv: 3rd arg is overwrite flag (1 = replace existing) */
    setenv("MY_DEMO_VAR", "hello_from_c", 1);
    printf("  After setenv: MY_DEMO_VAR = %s\n", getenv("MY_DEMO_VAR"));

    /* unsetenv removes it from this process's environment */
    unsetenv("MY_DEMO_VAR");
    printf("  After unsetenv: MY_DEMO_VAR = %s\n\n",
           getenv("MY_DEMO_VAR") ? getenv("MY_DEMO_VAR") : "(removed)");

    printf("  What happens: environment variables are inherited from\n");
    printf("  the parent process.  setenv/unsetenv only affect THIS\n");
    printf("  process and its future children — NEVER the parent shell.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Signal Handling
 * ════════════════════════════════════════════════════════════════ */

/* sig_atomic_t is the ONLY type guaranteed safe to read/write in a
 * signal handler.  volatile tells the compiler not to cache it. */
static volatile sig_atomic_t signal_received = 0;

/* Signal handler restrictions:
 *   - Can only call "async-signal-safe" functions (write, _exit, etc.)
 *   - printf is NOT async-signal-safe (uses malloc internally)
 *   - malloc is NOT async-signal-safe (not reentrant)
 *   - Best practice: set a flag and return, handle in main loop.
 *
 * We violate this rule with printf below for DEMO purposes only.
 * In production code, just set the flag. */
static void signal_handler(int sig)
{
    signal_received = sig;      /* safe: sig_atomic_t write is atomic */
    /* Don't call printf here in real code! */
}

static void demo_signals(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Signal Handling                         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Register handler for SIGUSR1 (user-defined signal) */
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);       /* don't block other signals during handler */
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);  /* sigaction is preferred over signal() */

    printf("  Registered handler for SIGUSR1.\n");
    printf("  Sending SIGUSR1 to self (PID %d)...\n", getpid());
    raise(SIGUSR1);                 /* send signal to ourselves */

    if (signal_received == SIGUSR1)
        printf("  Received SIGUSR1 (signal %d)!\n\n", signal_received);

    /* Restore default handler */
    sa.sa_handler = SIG_DFL;
    sigaction(SIGUSR1, &sa, NULL);

    printf("  Common signals:\n");
    printf("    SIGINT  (2)  — Ctrl+C, interrupt\n");
    printf("    SIGTERM (15) — polite kill request\n");
    printf("    SIGKILL (9)  — forced kill (cannot be caught!)\n");
    printf("    SIGSEGV (11) — segfault (invalid memory access)\n");
    printf("    SIGUSR1 (10) — user-defined\n\n");

    printf("  Async-signal-safe: Only write(), _exit(), signal() and\n");
    printf("  a few others are safe inside handlers.  printf, malloc,\n");
    printf("  and most library functions are NOT.  Set a flag instead.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Process Information
 * ════════════════════════════════════════════════════════════════ */

static void demo_process_info(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Process Information                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  PID:  %d  (this process)\n", getpid());
    printf("  PPID: %d  (parent — your shell or make)\n", getppid());
    printf("  UID:  %d  (user ID — 0 would mean root)\n", getuid());
    printf("  GID:  %d  (group ID)\n", getgid());

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)))
        printf("  CWD:  %s\n", cwd);
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Fork — Creating Child Processes
 * ════════════════════════════════════════════════════════════════ */

static void demo_fork(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Fork — Creating Child Processes         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  Parent PID: %d\n", getpid());

    /* fork() duplicates the entire process.
     * Copy-On-Write (COW): the OS doesn't actually copy memory pages
     * until one process writes to them.  This makes fork() fast even
     * for processes using gigabytes of memory.
     *
     * File descriptors are INHERITED: child gets copies of all open
     * FDs (stdin, stdout, files, sockets, pipes).  This is how shell
     * I/O redirection and piping work. */

    pid_t pid = fork();

    if (pid < 0) {
        perror("  fork failed");
    } else if (pid == 0) {
        /* --- CHILD PROCESS --- */
        printf("  [Child]  PID=%d, PPID=%d\n", getpid(), getppid());

        /* Use _exit() in the child, NOT exit().
         * exit() flushes stdio buffers, which could duplicate output
         * that the parent is also about to flush.
         * _exit() terminates immediately without flushing.          */
        _exit(42);      /* exit code returned to parent via wait */
    } else {
        /* --- PARENT PROCESS --- */
        int status;
        waitpid(pid, &status, 0);   /* block until child finishes */

        /* WIFEXITED: did the child exit normally (not killed by signal)?
         * WEXITSTATUS: the actual exit code (0–255).
         * These are macros that unpack the status word's bit fields. */
        if (WIFEXITED(status)) {
            printf("  [Parent] Child exited normally, code=%d\n",
                   WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("  [Parent] Child killed by signal %d\n",
                   WTERMSIG(status));
        }
    }

    printf("\n  What happens: fork() returns TWICE — once in each process.\n");
    printf("  Parent gets child's PID (>0).  Child gets 0.\n");
    printf("  COW means the copy is nearly free until someone writes.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: system() — Convenience vs Security
 * ════════════════════════════════════════════════════════════════ */

static void demo_system_cmd(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: system() Command Execution              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* system() spawns /bin/sh -c "command" — a full shell.
     * Convenient but DANGEROUS if the command string contains
     * user input.  An attacker could inject "; rm -rf /" etc.
     * Prefer exec*() family in production code. */

    printf("  Running 'uname -r':\n  ");
    int ret = system("uname -r");
    printf("  system() returned: %d\n\n", ret);

    printf("  Running 'date +%%Y-%%m-%%d':\n  ");
    ret = system("date +%Y-%m-%d");
    (void)ret;
    printf("\n");

    printf("  Security warning:\n");
    printf("    system(user_input) is a SHELL INJECTION vulnerability.\n");
    printf("    Example: if user_input = \"; rm -rf /\"\n");
    printf("    The shell executes: /bin/sh -c \"cmd; rm -rf /\"\n");
    printf("    Always use execvp() with explicit argv[] instead.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Pipes — Parent ↔ Child IPC
 * ════════════════════════════════════════════════════════════════ */

static void demo_pipe(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Pipes — Parent ↔ Child IPC              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* pipe() creates a unidirectional data channel:
     *   pipefd[0] = read end
     *   pipefd[1] = write end
     * The pipe exists in kernel memory.  Data written to fd[1]
     * becomes available for reading from fd[0].
     *
     * Combined with fork(), this is how shell pipes work:
     *   ls | grep foo  →  ls writes to pipe, grep reads from it. */

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("  pipe failed");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("  fork failed");
        return;
    }

    if (pid == 0) {
        /* --- CHILD: write to pipe --- */
        close(pipefd[0]);       /* child doesn't read — close read end */

        const char *msg = "Hello from child via pipe!";
        ssize_t bytes_written = write(pipefd[1], msg, strlen(msg) + 1);
        (void)bytes_written;    /* demo — real code should check this */

        close(pipefd[1]);       /* done writing — signals EOF to reader */
        _exit(0);
    } else {
        /* --- PARENT: read from pipe --- */
        close(pipefd[1]);       /* parent doesn't write — close write end */

        char buf[128] = {0};
        ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);

        if (n > 0)
            printf("  Parent received %zd bytes: \"%s\"\n", n, buf);

        close(pipefd[0]);
        waitpid(pid, NULL, 0);  /* reap child to avoid zombie */
    }
    printf("\n");

    printf("  What happens: pipe() creates two file descriptors.\n");
    printf("  After fork(), child has copies of both FDs.\n");
    printf("  Convention: close the end you don't use in each process.\n");
    printf("  This is the foundation of shell pipelines (cmd1 | cmd2).\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Time Functions
 * ════════════════════════════════════════════════════════════════ */

static void demo_time(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Time Functions                          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* time() returns seconds since Unix epoch (1970-01-01 00:00:00 UTC) */
    time_t now = time(NULL);
    printf("  time()    = %ld  (seconds since 1970-01-01 UTC)\n", (long)now);

    /* ctime: quick human-readable conversion (includes newline!) */
    printf("  ctime()   = %s", ctime(&now));     /* note: no \\n needed */

    /* localtime: break epoch into components (local timezone) */
    struct tm *local = localtime(&now);
    printf("  localtime = %04d-%02d-%02d %02d:%02d:%02d\n",
           local->tm_year + 1900,   /* years since 1900 */
           local->tm_mon + 1,       /* months 0–11, so add 1 */
           local->tm_mday,
           local->tm_hour, local->tm_min, local->tm_sec);

    /* strftime: the proper way to format time strings */
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", local);
    printf("  strftime  = %s\n\n", buf);

    printf("  Note: localtime returns a pointer to a STATIC buffer.\n");
    printf("  It is NOT thread-safe.  Use localtime_r() in threaded code.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n=== C SYSTEM PROGRAMMING DEMO v%s ===\n", VERSION_STRING);

    demo_environment();
    demo_signals();
    demo_process_info();
    demo_fork();
    demo_system_cmd();
    demo_pipe();
    demo_time();

    DEMO_END();
    return 0;
}
