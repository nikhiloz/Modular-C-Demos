/**
 * @file system.c
 * @brief C system programming: signals, environment, process
 */
#include "../../include/common.h"
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static volatile sig_atomic_t signal_received = 0;

void signal_handler(int sig) {
    signal_received = sig;
}

void demo_environment(void) {
    DEMO_SECTION("Environment Variables");
    char *home = getenv("HOME");
    char *path = getenv("PATH");
    printf("HOME = %s\n", home ? home : "(not set)");
    printf("PATH = %.50s...\n", path ? path : "(not set)");
    
    setenv("MY_VAR", "hello", 1);
    printf("MY_VAR = %s\n", getenv("MY_VAR"));
    unsetenv("MY_VAR");
}

void demo_signals(void) {
    DEMO_SECTION("Signal Handling");
    printf("Registering SIGUSR1 handler...\n");
    signal(SIGUSR1, signal_handler);
    printf("Sending SIGUSR1 to self...\n");
    raise(SIGUSR1);
    if (signal_received == SIGUSR1)
        printf("Received SIGUSR1!\n");
    signal(SIGUSR1, SIG_DFL);
}

void demo_process_info(void) {
    DEMO_SECTION("Process Information");
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("UID: %d\n", getuid());
    printf("GID: %d\n", getgid());
    
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)))
        printf("CWD: %s\n", cwd);
}

void demo_fork(void) {
    DEMO_SECTION("Fork and Process Creation");
    printf("Parent PID: %d\n", getpid());
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        printf("Child process PID: %d, PPID: %d\n", getpid(), getppid());
        _exit(42);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            printf("Child exited with code: %d\n", WEXITSTATUS(status));
    }
}

void demo_system_cmd(void) {
    DEMO_SECTION("System Commands");
    printf("Running  uname -a:\n");
    system("uname -a");
    printf("\nRunning date:\n");
    system("date");
}

void demo_time(void) {
    DEMO_SECTION("Time Functions");
    time_t now = time(NULL);
    printf("time(): %ld\n", (long)now);
    printf("ctime(): %s", ctime(&now));
    
    struct tm *local = localtime(&now);
    printf("localtime: %04d-%02d-%02d %02d:%02d:%02d\n",
           local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
           local->tm_hour, local->tm_min, local->tm_sec);
    
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", local);
    printf("strftime: %s\n", buf);
}

int main(void) {
    printf("\n=== C SYSTEM PROGRAMMING DEMO v%s ===\n", VERSION_STRING);
    demo_environment();
    demo_signals();
    demo_process_info();
    demo_fork();
    demo_system_cmd();
    demo_time();
    DEMO_END(); return 0;
}
