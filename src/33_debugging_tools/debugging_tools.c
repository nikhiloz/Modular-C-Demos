/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 33 — Debugging Tools & Techniques                      ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: GDB, Valgrind, sanitizers, strace, objdump            ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Intentional bugs for debugging demos ────────────────────── */

/* Memory leak */
static void demonstrate_leak(void)
{
    int *leaked = malloc(100 * sizeof(int));
    if (leaked) {
        leaked[0] = 42;
        printf("    Allocated %zu bytes at %p (intentionally not freed)\n",
               100 * sizeof(int), (void*)leaked);
    }
    /* NOTE: leaked is not freed — Valgrind will report this */
    free(leaked);  /* Actually free it so the demo is clean */
}

/* Buffer overread (safe version to avoid actual UB in demo) */
static void demonstrate_overread(void)
{
    int arr[5] = {10, 20, 30, 40, 50};
    printf("    Array contents (safe): ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");
    printf("    (In a real bug, accessing arr[5] or beyond = UB)\n");
}

/* Use-after-free pattern (explained, not executed unsafely) */
static void demonstrate_uaf_concept(void)
{
    printf("    Use-after-free pattern:\n");
    printf("      int *p = malloc(sizeof(int));\n");
    printf("      *p = 42;\n");
    printf("      free(p);\n");
    printf("      *p = 99;    ← UNDEFINED BEHAVIOUR\n");
    printf("    AddressSanitizer catches this immediately.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Debugging Strategy
 * ════════════════════════════════════════════════════════════════════ */
static void demo_strategy(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Debugging Strategy                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  0. Compile with warnings: gcc -Wall -Wextra -Werror\n");
    printf("  1. Reproduce the bug reliably.\n");
    printf("  2. Minimise the test case.\n");
    printf("  3. Use the right tool:\n\n");

    printf("  ┌────────────────────────┬─────────────────────────────┐\n");
    printf("  │ Problem                │ Tool                        │\n");
    printf("  ├────────────────────────┼─────────────────────────────┤\n");
    printf("  │ Crash / segfault       │ GDB, AddressSanitizer       │\n");
    printf("  │ Memory leak            │ Valgrind, LeakSanitizer     │\n");
    printf("  │ Buffer overflow        │ AddressSanitizer             │\n");
    printf("  │ Data race              │ ThreadSanitizer              │\n");
    printf("  │ Undefined behaviour    │ UBSan                       │\n");
    printf("  │ System call issues     │ strace                      │\n");
    printf("  │ Library call issues    │ ltrace                      │\n");
    printf("  │ Performance            │ perf, gprof, cachegrind     │\n");
    printf("  │ Binary analysis        │ objdump, readelf, nm        │\n");
    printf("  └────────────────────────┴─────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — GDB (GNU Debugger)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_gdb(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — GDB (GNU Debugger)                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Compile with debug info: gcc -g -O0 program.c\n\n");

    printf("  Essential GDB commands:\n");
    printf("  ┌─────────────────────┬──────────────────────────────────┐\n");
    printf("  │ Command             │ Action                           │\n");
    printf("  ├─────────────────────┼──────────────────────────────────┤\n");
    printf("  │ gdb ./program       │ Start GDB                        │\n");
    printf("  │ run [args]          │ Run the program                  │\n");
    printf("  │ break main          │ Set breakpoint at main           │\n");
    printf("  │ break file.c:42     │ Break at line 42                 │\n");
    printf("  │ next (n)            │ Step over function               │\n");
    printf("  │ step (s)            │ Step into function               │\n");
    printf("  │ continue (c)        │ Continue running                 │\n");
    printf("  │ print expr (p)      │ Print expression value           │\n");
    printf("  │ backtrace (bt)      │ Show call stack                  │\n");
    printf("  │ info locals         │ Show local variables             │\n");
    printf("  │ info registers      │ Show CPU registers               │\n");
    printf("  │ watch var           │ Break when var changes           │\n");
    printf("  │ x/10x addr          │ Examine memory (hex)             │\n");
    printf("  │ disassemble func    │ Show assembly                    │\n");
    printf("  │ quit                │ Exit GDB                         │\n");
    printf("  └─────────────────────┴──────────────────────────────────┘\n\n");

    printf("  GDB with core dump:\n");
    printf("    ulimit -c unlimited\n");
    printf("    ./program   ← crashes, produces 'core'\n");
    printf("    gdb ./program core\n");
    printf("    (gdb) bt    ← see where it crashed\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Valgrind
 * ════════════════════════════════════════════════════════════════════ */
static void demo_valgrind(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Valgrind (Memory Checker)                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  valgrind --leak-check=full ./program\n\n");

    printf("  Valgrind detects:\n");
    printf("    - Memory leaks (definitely lost, indirectly lost)\n");
    printf("    - Use of uninitialised memory\n");
    printf("    - Reads/writes past allocated blocks\n");
    printf("    - Use-after-free\n");
    printf("    - Double free\n");
    printf("    - Mismatched malloc/free vs new/delete\n\n");

    printf("  Demo: running leak demonstration...\n");
    demonstrate_leak();
    printf("    (Run with: valgrind --leak-check=full ./debugging_tools)\n\n");

    printf("  Valgrind tools:\n");
    printf("  ┌────────────────┬─────────────────────────────────────┐\n");
    printf("  │ Tool           │ Purpose                             │\n");
    printf("  ├────────────────┼─────────────────────────────────────┤\n");
    printf("  │ memcheck       │ Memory errors (default)             │\n");
    printf("  │ callgrind      │ Call graph + instruction count     │\n");
    printf("  │ cachegrind     │ Cache/branch prediction profiling  │\n");
    printf("  │ massif         │ Heap profiler                      │\n");
    printf("  │ helgrind       │ Thread error detector              │\n");
    printf("  │ drd            │ Data race detector                 │\n");
    printf("  └────────────────┴─────────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Compiler Sanitizers
 * ════════════════════════════════════════════════════════════════════ */
static void demo_sanitizers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Compiler Sanitizers                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Built into GCC and Clang — faster than Valgrind!\n\n");

    printf("  ┌───────────────────────────┬────────────────────────────────┐\n");
    printf("  │ Flag                      │ Detects                        │\n");
    printf("  ├───────────────────────────┼────────────────────────────────┤\n");
    printf("  │ -fsanitize=address (ASan) │ Buffer overflow, use-after-free│\n");
    printf("  │                           │ stack overflow, memory leaks   │\n");
    printf("  │ -fsanitize=undefined      │ Signed overflow, null deref,   │\n");
    printf("  │  (UBSan)                  │ shift errors, type mismatch   │\n");
    printf("  │ -fsanitize=thread (TSan)  │ Data races, deadlocks         │\n");
    printf("  │ -fsanitize=leak           │ Memory leaks (standalone)      │\n");
    printf("  └───────────────────────────┴────────────────────────────────┘\n\n");

    printf("  Usage:\n");
    printf("    gcc -g -fsanitize=address,undefined -o prog prog.c\n");
    printf("    ./prog   ← crashes with detailed error report\n\n");

    printf("  Demo concept: use-after-free detection:\n");
    demonstrate_uaf_concept();
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — strace & ltrace
 * ════════════════════════════════════════════════════════════════════ */
static void demo_strace(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — strace & ltrace                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  strace — trace system calls:\n");
    printf("    strace ./program\n");
    printf("    strace -e trace=open,read,write ./program\n");
    printf("    strace -f ./program   (follow forks)\n");
    printf("    strace -c ./program   (summary: counts/times)\n\n");

    printf("  ltrace — trace library calls:\n");
    printf("    ltrace ./program\n");
    printf("    ltrace -e malloc+free ./program\n\n");

    printf("  Demo: array contents (safe access):\n");
    demonstrate_overread();
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Binary Analysis Tools
 * ════════════════════════════════════════════════════════════════════ */
static void demo_binary_tools(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Binary Analysis Tools                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌─────────────────┬──────────────────────────────────────┐\n");
    printf("  │ Tool            │ Purpose                              │\n");
    printf("  ├─────────────────┼──────────────────────────────────────┤\n");
    printf("  │ objdump -d      │ Disassemble .text section            │\n");
    printf("  │ objdump -s      │ Hex dump of all sections             │\n");
    printf("  │ readelf -h      │ ELF header                           │\n");
    printf("  │ readelf -S      │ Section headers                      │\n");
    printf("  │ readelf -s      │ Symbol table                         │\n");
    printf("  │ readelf -l      │ Program headers (segments)           │\n");
    printf("  │ readelf -d      │ Dynamic section                      │\n");
    printf("  │ readelf -r      │ Relocations                          │\n");
    printf("  │ nm              │ List symbols (simpler than readelf)   │\n");
    printf("  │ ldd             │ List shared library dependencies     │\n");
    printf("  │ file            │ Identify file type                   │\n");
    printf("  │ size            │ Section sizes (.text/.data/.bss)     │\n");
    printf("  │ strings         │ Print readable strings in binary     │\n");
    printf("  │ hexdump -C      │ Raw hex + ASCII dump                 │\n");
    printf("  └─────────────────┴──────────────────────────────────────┘\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Quick analysis of this binary:                         ║\n");
    printf("║  file ./debugging_tools                                 ║\n");
    printf("║  size ./debugging_tools                                 ║\n");
    printf("║  nm ./debugging_tools | head -20                        ║\n");
    printf("║  strings ./debugging_tools | grep -i chapter            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Compiler Warning Flags
 * ════════════════════════════════════════════════════════════════════ */
static void demo_warnings(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Compiler Warning Flags                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Essential flags:\n");
    printf("    gcc -Wall -Wextra -Wpedantic -Wshadow\n");
    printf("        -Wformat=2 -Wconversion -Werror\n\n");

    printf("  ┌─────────────────────┬──────────────────────────────────┐\n");
    printf("  │ Flag                │ Catches                          │\n");
    printf("  ├─────────────────────┼──────────────────────────────────┤\n");
    printf("  │ -Wall               │ Common warnings                  │\n");
    printf("  │ -Wextra             │ Extra warnings beyond -Wall      │\n");
    printf("  │ -Wpedantic          │ Strict ISO C compliance          │\n");
    printf("  │ -Wshadow            │ Variable shadowing               │\n");
    printf("  │ -Wformat=2          │ printf format string issues      │\n");
    printf("  │ -Wconversion        │ Implicit type conversions        │\n");
    printf("  │ -Werror             │ Treat warnings as errors         │\n");
    printf("  │ -Wstrict-prototypes │ Missing function prototypes      │\n");
    printf("  │ -Wno-unused-param   │ Suppress unused parameter        │\n");
    printf("  └─────────────────────┴──────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 33 — Debugging Tools & Techniques                  ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_strategy();
    demo_gdb();
    demo_valgrind();
    demo_sanitizers();
    demo_strace();
    demo_binary_tools();
    demo_warnings();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 33 — Debugging Tools & Techniques\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
