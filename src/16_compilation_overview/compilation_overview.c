/*
 * Chapter 16 — Compilation Overview
 *
 * Demonstrates the 4 stages of C compilation:
 *   1. Preprocessing  (cpp)   : .c  → .i
 *   2. Compilation    (cc1)   : .i  → .s
 *   3. Assembly       (as)    : .s  → .o
 *   4. Linking        (ld)    : .o  → executable
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/16_compilation_overview \
 *            src/16_compilation_overview/compilation_overview.c
 * Run:   ./bin/16_compilation_overview
 *
 * Try these to see each stage:
 *   gcc -E  src/16_compilation_overview/compilation_overview.c -o /tmp/stage1.i
 *   gcc -S  src/16_compilation_overview/compilation_overview.c -o /tmp/stage2.s
 *   gcc -c  src/16_compilation_overview/compilation_overview.c -o /tmp/stage3.o
 *   gcc     src/16_compilation_overview/compilation_overview.c -o /tmp/stage4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ════════════════════════════════════════════════════════════════
 *  Macros to demonstrate preprocessing
 * ════════════════════════════════════════════════════════════════ */

#define GREETING "Hello from compilation overview!"
#define SQUARE(x) ((x) * (x))
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

/* Conditional compilation — demonstrates #ifdef */
#ifdef DEBUG
#define DBG_PRINT(fmt, ...) fprintf(stderr, "[DBG] " fmt "\n", ##__VA_ARGS__)
#else
#define DBG_PRINT(fmt, ...) ((void)0)
#endif

/* ════════════════════════════════════════════════════════════════
 *  Stage 1: Preprocessing demo
 * ════════════════════════════════════════════════════════════════ */
static void demo_preprocessing(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Stage 1: PREPROCESSING (cpp / gcc -E)             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("What happens:\n");
    printf("  • #include directives → header content inserted\n");
    printf("  • #define macros → text substitution\n");
    printf("  • #ifdef/#ifndef → conditional code selection\n");
    printf("  • #pragma → compiler-specific hints\n");
    printf("  • Comments removed\n");
    printf("  • Line markers added for error reporting\n\n");

    printf("Demo — macro expansion results:\n");
    printf("  GREETING        = \"%s\"\n", GREETING);
    printf("  SQUARE(5)       = %d\n", SQUARE(5));
    printf("  SQUARE(2+3)     = %d  (parenthesised correctly)\n", SQUARE(2+3));
    printf("  VERSION          = %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    printf("  __FILE__         = \"%s\"\n", __FILE__);
    printf("  __LINE__         = %d\n", __LINE__);
    printf("  __DATE__         = \"%s\"\n", __DATE__);
    printf("  __STDC_VERSION__ = %ldL\n\n", __STDC_VERSION__);

    printf("Try: gcc -E %s | head -50\n", __FILE__);
    printf("     (shows the .i preprocessed output)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Stage 2: Compilation demo (source → assembly)
 * ════════════════════════════════════════════════════════════════ */

/* A simple function whose assembly is easy to read */
int add_numbers(int a, int b)
{
    return a + b;
}

int factorial_iterative(int n)
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

static void demo_compilation(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Stage 2: COMPILATION (cc1 / gcc -S)               ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("What happens:\n");
    printf("  • Lexical analysis    → tokens\n");
    printf("  • Syntax analysis     → abstract syntax tree (AST)\n");
    printf("  • Semantic analysis   → type checking, symbol resolution\n");
    printf("  • IR generation       → intermediate representation\n");
    printf("  • Optimisation passes → constant folding, DCE, inlining\n");
    printf("  • Code generation     → assembly (.s file)\n\n");

    printf("Demo — calling functions that compile to simple assembly:\n");
    printf("  add_numbers(3, 7)     = %d\n", add_numbers(3, 7));
    printf("  factorial_iterative(6)= %d\n\n", factorial_iterative(6));

    printf("Try: gcc -S -O0 %s -o /tmp/stage2_O0.s\n", __FILE__);
    printf("     gcc -S -O2 %s -o /tmp/stage2_O2.s\n", __FILE__);
    printf("     diff /tmp/stage2_O0.s /tmp/stage2_O2.s\n");
    printf("     (compare unoptimised vs optimised assembly)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Stage 3: Assembly demo (.s → .o)
 * ════════════════════════════════════════════════════════════════ */
static void demo_assembly(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Stage 3: ASSEMBLY (as / gcc -c)                   ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("What happens:\n");
    printf("  • Assembly mnemonics → machine code bytes\n");
    printf("  • Creates ELF object file (.o)\n");
    printf("  • Sections: .text (code), .data, .bss, .rodata\n");
    printf("  • Symbol table: exported and undefined symbols\n");
    printf("  • Relocation entries: addresses to fix at link time\n\n");

    printf("Try: gcc -c %s -o /tmp/stage3.o\n", __FILE__);
    printf("     objdump -d /tmp/stage3.o | head -40\n");
    printf("     readelf -S /tmp/stage3.o\n");
    printf("     nm /tmp/stage3.o\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Stage 4: Linking demo (.o → executable)
 * ════════════════════════════════════════════════════════════════ */
static void demo_linking(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Stage 4: LINKING (ld / gcc)                       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("What happens:\n");
    printf("  • Combine multiple .o files into one executable\n");
    printf("  • Resolve external symbols (e.g., printf → libc)\n");
    printf("  • Apply relocations → fix absolute addresses\n");
    printf("  • Add CRT startup code (_start → main)\n");
    printf("  • Create program headers for OS loader\n");
    printf("  • Output: ELF executable (or shared library)\n\n");

    printf("Try: gcc -v %s -o /tmp/stage4 2>&1 | tail -10\n", __FILE__);
    printf("     (shows the actual ld command GCC invokes)\n");
    printf("     file /tmp/stage4\n");
    printf("     ldd /tmp/stage4\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Summary: gcc -save-temps
 * ════════════════════════════════════════════════════════════════ */
static void demo_save_temps(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  All-in-one: gcc -save-temps                       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("This single flag saves all intermediate files:\n\n");
    printf("  gcc -save-temps %s -o /tmp/all_stages\n\n", __FILE__);
    printf("Generated files:\n");
    printf("  compilation_overview.i   ← preprocessed source\n");
    printf("  compilation_overview.s   ← assembly\n");
    printf("  compilation_overview.o   ← object file\n");
    printf("  /tmp/all_stages          ← final executable\n\n");

    printf("Inspect each:\n");
    printf("  wc -l compilation_overview.i   (hundreds of lines after #include)\n");
    printf("  cat compilation_overview.s     (assembly for this file)\n");
    printf("  objdump -t compilation_overview.o  (symbol table)\n");
    printf("  readelf -h /tmp/all_stages     (ELF header)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main
 * ════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 16 — Compilation Overview                 ║\n");
    printf("║  The 4 Stages: Preprocess → Compile → Assemble →  ║\n");
    printf("║                Link                                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    demo_preprocessing();
    demo_compilation();
    demo_assembly();
    demo_linking();
    demo_save_temps();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: .c → (cpp) → .i → (cc1) → .s → (as) → .o → (ld) → ELF\n");
    printf("════════════════════════════════════════════════════════\n");

    return 0;
}
