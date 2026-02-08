/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 30 — CRT Startup: From _start to main()               ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: _start, __libc_start_main, .init_array, argc/argv     ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Constructor / Destructor demos ──────────────────────────── */

__attribute__((constructor(101)))
static void early_init(void)
{
    /* Priority 101 — runs before default (65535) constructors.
     * (Priorities 0-100 are reserved for the implementation.) */
    /* printf("  [early_init] constructor priority 101\n"); */
}

__attribute__((constructor))
static void normal_init(void)
{
    /* Default priority constructor. */
    /* printf("  [normal_init] constructor default priority\n"); */
}

__attribute__((destructor))
static void cleanup(void)
{
    /* Runs after main() returns or exit() is called. */
    /* printf("  [cleanup] destructor running\n"); */
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — The Journey Before main()
 * ════════════════════════════════════════════════════════════════════ */
static void demo_before_main(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — The Journey Before main()                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When the kernel finishes loading your program, it does NOT\n");
    printf("jump to main(). It jumps to _start.\n\n");

    printf("  Execution order:\n\n");
    printf("  ┌──────────────────────────┐\n");
    printf("  │  Kernel: execve()        │\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  ld-linux.so (if dyn.)   │  Loads shared libs, resolves symbols\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  _start  (crt1.o)        │  Entry point (in CRT)\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  __libc_start_main()     │  Sets up glibc internals\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  __libc_csu_init()       │  Calls .init_array functions\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  main(argc, argv, envp)  │  YOUR CODE\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  exit()                  │  Calls .fini_array, atexit handlers\n");
    printf("  └────────────┬─────────────┘\n");
    printf("               │\n");
    printf("  ┌────────────▼─────────────┐\n");
    printf("  │  _exit()  (syscall)      │  Process terminates\n");
    printf("  └──────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — _start (crt1.o)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_start(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — _start (crt1.o)                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("_start is provided by crt1.o (C Runtime startup).\n");
    printf("It is the actual entry point of the ELF executable.\n\n");

    printf("  Simplified x86-64 _start (AT&T syntax):\n\n");
    printf("  _start:\n");
    printf("      xor  %%ebp, %%ebp          # Clear frame pointer (ABI)\n");
    printf("      mov  (%%rsp), %%rdi        # rdi = argc\n");
    printf("      lea  8(%%rsp), %%rsi       # rsi = argv\n");
    printf("      lea  16(%%rsp,%%rdi,8), %%rdx  # rdx = envp\n");
    printf("      call __libc_start_main\n");
    printf("      hlt                       # should never reach here\n\n");

    printf("  CRT files linked automatically by GCC:\n");
    printf("  ┌───────────────┬──────────────────────────────────────┐\n");
    printf("  │ File          │ Purpose                              │\n");
    printf("  ├───────────────┼──────────────────────────────────────┤\n");
    printf("  │ crt1.o        │ Contains _start, calls __libc_start_main│\n");
    printf("  │ crti.o        │ .init/.fini section prologue         │\n");
    printf("  │ crtn.o        │ .init/.fini section epilogue         │\n");
    printf("  │ crtbegin.o    │ GCC frame registration (.eh_frame)   │\n");
    printf("  │ crtend.o      │ GCC frame registration end           │\n");
    printf("  └───────────────┴──────────────────────────────────────┘\n\n");

    printf("  See it: gcc -v main.c 2>&1 | grep crt\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — __libc_start_main
 * ════════════════════════════════════════════════════════════════════ */
static void demo_libc_start(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — __libc_start_main                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("__libc_start_main (in glibc) does setup before calling main():\n\n");

    printf("  int __libc_start_main(\n");
    printf("      int (*main)(int, char**, char**),  // pointer to main\n");
    printf("      int argc,\n");
    printf("      char **argv,\n");
    printf("      void (*init)(void),    // __libc_csu_init\n");
    printf("      void (*fini)(void),    // __libc_csu_fini\n");
    printf("      void (*rtld_fini)(void), // ld.so cleanup\n");
    printf("      void *stack_end        // bottom of stack\n");
    printf("  );\n\n");

    printf("  What it does:\n");
    printf("    1. Set up thread-local storage (TLS).\n");
    printf("    2. Register rtld_fini with atexit (for ld.so cleanup).\n");
    printf("    3. Register fini with atexit (.fini_array).\n");
    printf("    4. Call init (→ .preinit_array, .init, .init_array).\n");
    printf("    5. Call main(argc, argv, envp).\n");
    printf("    6. Call exit(main's return value).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — .init_array and .fini_array
 * ════════════════════════════════════════════════════════════════════ */
static void demo_init_fini(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — .init_array and .fini_array                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  .init_array = table of function pointers called before main().\n");
    printf("  .fini_array = table of function pointers called after main().\n\n");

    printf("  Creating constructors/destructors:\n\n");
    printf("    __attribute__((constructor))\n");
    printf("    void my_init(void) { /* before main */ }\n\n");
    printf("    __attribute__((constructor(101)))\n");
    printf("    void early_init(void) { /* lower = earlier */ }\n\n");
    printf("    __attribute__((destructor))\n");
    printf("    void my_fini(void) { /* after main */ }\n\n");

    printf("  Execution order:\n");
    printf("    .preinit_array (main executable only)\n");
    printf("    .init          (legacy, from crti.o)\n");
    printf("    .init_array    (constructors, in order)\n");
    printf("    ── main() runs ─────────────────────\n");
    printf("    atexit handlers (LIFO order)\n");
    printf("    .fini_array    (destructors, reverse order)\n");
    printf("    .fini          (legacy, from crtn.o)\n\n");

    printf("  Legacy C++ uses .init_array for global object constructors\n");
    printf("  and .fini_array for global object destructors.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — atexit() Handlers
 * ════════════════════════════════════════════════════════════════════ */

static void atexit_handler_1(void) { printf("  [atexit handler 1]\n"); }
static void atexit_handler_2(void) { printf("  [atexit handler 2]\n"); }

static void demo_atexit(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — atexit() Handlers                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  atexit() registers functions to run when the process exits.\n");
    printf("  They run in LIFO (last registered, first called) order.\n\n");

    printf("  Registering two atexit handlers...\n");
    atexit(atexit_handler_1);  /* called second (registered first) */
    atexit(atexit_handler_2);  /* called first  (registered last) */

    printf("  (You'll see them print after 'End of Chapter 30'.)\n\n");

    printf("  Notes:\n");
    printf("    - Max 32 (POSIX minimum) to 2048+ handlers.\n");
    printf("    - exit() calls them; _exit() / _Exit() do NOT.\n");
    printf("    - abort() does NOT call atexit handlers.\n");
    printf("    - Handlers should not call exit() (undefined behaviour).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Bypassing CRT (_start without glibc)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_no_crt(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Bypassing CRT (Freestanding _start)        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  You can write a program without any C runtime:\n\n");

    printf("  /* nostdlib.c — no libc, no CRT */\n");
    printf("  void _start(void) {\n");
    printf("      /* Write using raw syscall */\n");
    printf("      const char msg[] = \"Hello, raw!\\n\";\n");
    printf("      asm volatile(\n");
    printf("          \"mov $1, %%%%rax\\n\"   /* sys_write */\n");
    printf("          \"mov $1, %%%%rdi\\n\"   /* fd = stdout */\n");
    printf("          \"mov %%0, %%%%rsi\\n\"  /* buf */\n");
    printf("          \"mov $12, %%%%rdx\\n\"  /* len */\n");
    printf("          \"syscall\\n\"\n");
    printf("          \"mov $60, %%%%rax\\n\"  /* sys_exit */\n");
    printf("          \"xor %%%%rdi, %%%%rdi\\n\" /* status = 0 */\n");
    printf("          \"syscall\"\n");
    printf("          : : \"r\"(msg) : \"rax\",\"rdi\",\"rsi\",\"rdx\"\n");
    printf("      );\n");
    printf("  }\n\n");

    printf("  Compile: gcc -nostdlib -o nostdlib nostdlib.c\n");
    printf("  Size:    ~8 KB (vs ~16 KB with CRT)\n\n");

    printf("  Use cases:\n");
    printf("    - Embedded systems (no OS/libc available)\n");
    printf("    - OS kernel entry point\n");
    printf("    - Size-optimised programs (demoscene, ctf)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Examining CRT
 * ════════════════════════════════════════════════════════════════════ */
static void demo_examine_crt(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Examining CRT                               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  See CRT files in link command:                         ║\n");
    printf("║  gcc -v crt_startup.c 2>&1 | grep crt                  ║\n");
    printf("║                                                         ║\n");
    printf("║  Find _start in your binary:                            ║\n");
    printf("║  nm ./crt_startup | grep ' _start'                      ║\n");
    printf("║                                                         ║\n");
    printf("║  Disassemble _start:                                    ║\n");
    printf("║  objdump -d ./crt_startup | grep -A20 '<_start>'       ║\n");
    printf("║                                                         ║\n");
    printf("║  See .init_array contents:                              ║\n");
    printf("║  readelf -x .init_array ./crt_startup                   ║\n");
    printf("║  readelf -x .fini_array ./crt_startup                   ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace startup calls:                                   ║\n");
    printf("║  strace -e trace=write ./crt_startup                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[])
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 30 — CRT Startup: From _start to main()           ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    printf("\n  (main() received: argc=%d, argv[0]=\"%s\")\n", argc, argv[0]);

    demo_before_main();
    demo_start();
    demo_libc_start();
    demo_init_fini();
    demo_atexit();
    demo_no_crt();
    demo_examine_crt();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 30 — CRT Startup\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
