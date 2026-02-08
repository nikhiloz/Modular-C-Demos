/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 27 — Kernel exec: From Shell to Process                ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: fork/exec, execve syscall, kernel loader steps         ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What Happens When You Type ./program ?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_what_happens(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What Happens When You Type ./program?      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When you execute a program from the shell:\n\n");

    printf("  1. Shell calls fork() → creates child process.\n");
    printf("  2. Child calls execve(\"./program\", argv, envp).\n");
    printf("  3. Kernel opens the file and reads the first bytes.\n");
    printf("  4. Kernel checks the magic number:\n");
    printf("       - 7f 45 4c 46 → ELF binary\n");
    printf("       - 23 21       → #! (script — use interpreter)\n");
    printf("  5. For ELF: kernel invokes the ELF loader.\n");
    printf("  6. Kernel maps segments into the new address space.\n");
    printf("  7. If dynamically linked: kernel loads ld-linux.so.\n");
    printf("  8. Kernel sets up the stack (argc, argv, envp, auxv).\n");
    printf("  9. Kernel transfers control to the entry point.\n\n");

    printf("  Timeline:\n");
    printf("  ┌────────────────┐     ┌──────────────┐     ┌───────────┐\n");
    printf("  │ Shell (bash)   │────►│ fork()        │────►│ execve()  │\n");
    printf("  └────────────────┘     └──────────────┘     └─────┬─────┘\n");
    printf("                                                    │\n");
    printf("                         ┌──────────────────────────▼───────┐\n");
    printf("                         │ Kernel: open file, parse ELF,    │\n");
    printf("                         │ map segments, set up stack,      │\n");
    printf("                         │ transfer to _start / ld.so       │\n");
    printf("                         └──────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — fork() + exec() Pattern
 * ════════════════════════════════════════════════════════════════════ */
static void demo_fork_exec(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — fork() + exec() Pattern                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  fork() creates a copy of the current process.\n");
    printf("  exec() replaces the copy's memory with a new program.\n\n");

    printf("  pid_t pid = fork();\n");
    printf("  if (pid == 0) {\n");
    printf("      // Child process\n");
    printf("      execve(\"/bin/ls\", argv, envp);\n");
    printf("      perror(\"exec failed\");  // only reached on error\n");
    printf("      _exit(1);\n");
    printf("  } else {\n");
    printf("      // Parent process\n");
    printf("      waitpid(pid, &status, 0);\n");
    printf("  }\n\n");

    printf("  The exec family:\n");
    printf("  ┌───────────┬──────────────────────────────────────────┐\n");
    printf("  │ Function  │ Description                              │\n");
    printf("  ├───────────┼──────────────────────────────────────────┤\n");
    printf("  │ execve    │ System call (path, argv[], envp[])       │\n");
    printf("  │ execv     │ Like execve, inherits environment        │\n");
    printf("  │ execvp    │ Searches PATH for the program            │\n");
    printf("  │ execl     │ Variadic arg list instead of argv[]      │\n");
    printf("  │ execlp    │ Variadic + PATH search                   │\n");
    printf("  │ execle    │ Variadic + explicit envp[]               │\n");
    printf("  └───────────┴──────────────────────────────────────────┘\n\n");

    printf("  All exec variants ultimately call the execve syscall.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Live fork+exec Demo
 * ════════════════════════════════════════════════════════════════════ */
static void demo_live_fork(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Live fork+exec Demo                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Forking this process and running 'echo' in the child...\n\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("  fork failed");
        return;
    }
    if (pid == 0) {
        /* Child process */
        char *argv[] = {"echo", "  Hello from child process!", NULL};
        char *envp[] = {NULL};
        execve("/bin/echo", argv, envp);
        perror("  execve failed");
        _exit(1);
    } else {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        printf("  Parent: child exited with status %d\n", WEXITSTATUS(status));
        printf("  Parent PID = %d,  Child PID was = %d\n\n", getpid(), pid);
    }
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — The execve System Call (Kernel Side)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_execve_kernel(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — execve: Kernel-Side Steps                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When the kernel receives the execve() syscall:\n\n");

    printf("  1. Open the file, check permissions (executable bit).\n");
    printf("  2. Read the first 128+ bytes to identify format.\n");
    printf("  3. Search binary format handlers:\n");
    printf("       - binfmt_elf  → ELF binaries\n");
    printf("       - binfmt_script → #! scripts\n");
    printf("       - binfmt_misc → user-registered formats\n\n");

    printf("  For ELF (load_elf_binary in fs/binfmt_elf.c):\n");
    printf("  4. Parse ELF header and program headers.\n");
    printf("  5. Check architecture matches running kernel.\n");
    printf("  6. Flush old address space (mm_struct).\n");
    printf("  7. Map PT_LOAD segments:\n");
    printf("       mmap(vaddr, size, prot, MAP_PRIVATE|MAP_FIXED, fd, offset)\n");
    printf("  8. Set up BSS (zero-filled anonymous mapping).\n");
    printf("  9. If PT_INTERP: load dynamic linker (ld-linux.so).\n");
    printf("  10. Set up initial stack:\n");
    printf("       [argc][argv pointers][NULL][envp pointers][NULL][auxv]\n");
    printf("  11. Set instruction pointer to entry point.\n");
    printf("  12. Return to user space → program begins running.\n\n");

    printf("  Kernel source: fs/binfmt_elf.c (load_elf_binary)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Initial Stack Layout
 * ════════════════════════════════════════════════════════════════════ */
static void demo_initial_stack(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Initial Stack Layout                       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The kernel sets up the stack before the program runs:\n\n");

    printf("  High address (stack top)\n");
    printf("  ┌───────────────────────────┐\n");
    printf("  │ Environment strings       │ \"PATH=/usr/bin:...\"\n");
    printf("  │ Argument strings          │ \"./program\", \"arg1\"\n");
    printf("  ├───────────────────────────┤\n");
    printf("  │ NULL (end of auxv)        │\n");
    printf("  │ Auxiliary vector (auxv)   │ AT_ENTRY, AT_PHDR, ...\n");
    printf("  ├───────────────────────────┤\n");
    printf("  │ NULL (end of envp)        │\n");
    printf("  │ envp[1]                   │ pointer to env string\n");
    printf("  │ envp[0]                   │ pointer to env string\n");
    printf("  ├───────────────────────────┤\n");
    printf("  │ NULL (end of argv)        │\n");
    printf("  │ argv[1]                   │ pointer to \"arg1\"\n");
    printf("  │ argv[0]                   │ pointer to \"./program\"\n");
    printf("  ├───────────────────────────┤\n");
    printf("  │ argc                      │ number of arguments\n");
    printf("  └───────────────────────────┘  ← rsp points here\n");
    printf("  Low address\n\n");

    printf("  Auxiliary vector entries (examples):\n");
    printf("    AT_ENTRY  = program entry point address\n");
    printf("    AT_PHDR   = address of program header table\n");
    printf("    AT_PHNUM  = number of program headers\n");
    printf("    AT_PAGESZ = system page size (4096)\n");
    printf("    AT_RANDOM = address of 16 random bytes\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  View auxiliary vector:                                  ║\n");
    printf("║  LD_SHOW_AUXV=1 ./kernel_exec                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Script Execution (#!)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_shebang(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Script Execution (#!)                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When the first two bytes are '#!' (0x23 0x21):\n\n");
    printf("  #!/usr/bin/python3\n");
    printf("  print('hello')\n\n");

    printf("  The kernel transforms:\n");
    printf("    execve(\"./script.py\", [\"./script.py\"], envp)\n");
    printf("  into:\n");
    printf("    execve(\"/usr/bin/python3\", [\"/usr/bin/python3\", \"./script.py\"], envp)\n\n");

    printf("  Then starts over with the interpreter as a new execve.\n");
    printf("  This is handled by binfmt_script in the kernel.\n\n");

    printf("  Shebang rules:\n");
    printf("    - Max one optional argument: #!/usr/bin/env python3\n");
    printf("    - Max line length ~256 bytes (kernel limit).\n");
    printf("    - File must have executable permission.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Tracing exec
 * ════════════════════════════════════════════════════════════════════ */
static void demo_tracing(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Tracing Execution                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Trace system calls during exec:                        ║\n");
    printf("║  strace -f -e trace=execve,openat,mmap ./kernel_exec   ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace library loading:                                 ║\n");
    printf("║  LD_DEBUG=libs ./kernel_exec                            ║\n");
    printf("║                                                         ║\n");
    printf("║  Show auxiliary vector:                                 ║\n");
    printf("║  LD_SHOW_AUXV=1 ./kernel_exec                          ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace with ltrace (library calls):                     ║\n");
    printf("║  ltrace ./kernel_exec                                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[], char *envp[])
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 27 — Kernel exec: From Shell to Process            ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* Show argc/argv to demonstrate kernel stack setup */
    printf("\n  This process received from the kernel:\n");
    printf("    argc = %d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("    argv[%d] = \"%s\"\n", i, argv[i]);
    printf("    PID = %d\n", getpid());

    /* Count environment variables */
    int env_count = 0;
    while (envp[env_count]) env_count++;
    printf("    envp count = %d environment variables\n", env_count);

    demo_what_happens();
    demo_fork_exec();
    demo_live_fork();
    demo_execve_kernel();
    demo_initial_stack();
    demo_shebang();
    demo_tracing();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 27 — Kernel exec\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
