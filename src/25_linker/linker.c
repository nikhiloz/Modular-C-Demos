/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 25 — The Linker                                        ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: Symbol resolution, relocation, static/dynamic linking  ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Simulated "multi-file" symbols for demonstration ─────────── */

/* Strong global symbol */
int global_counter = 100;

/* Weak symbol — can be overridden by another .o file's strong def */
int __attribute__((weak)) default_value = 0;

/* Extern declaration — resolved by the linker (from libc) */
/* extern int printf(const char *fmt, ...); — already via stdio.h */

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What the Linker Does
 * ════════════════════════════════════════════════════════════════════ */
static void demo_linker_overview(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What the Linker Does                       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The linker (ld) combines multiple object files (.o) into\n");
    printf("a single executable (or shared library).\n\n");

    printf("Two main jobs:\n");
    printf("  1. Symbol Resolution — match each symbol reference to\n");
    printf("     exactly one definition.\n");
    printf("  2. Relocation — assign final addresses and patch all\n");
    printf("     references to their resolved addresses.\n\n");

    printf("  Linking pipeline:\n");
    printf("  ┌─────────┐  ┌─────────┐  ┌─────────┐\n");
    printf("  │ main.o  │  │ math.o  │  │ libc.a  │\n");
    printf("  └────┬────┘  └────┬────┘  └────┬────┘\n");
    printf("       │            │            │\n");
    printf("       └────────────┼────────────┘\n");
    printf("                    │\n");
    printf("              ┌─────▼──────┐\n");
    printf("              │  Linker    │\n");
    printf("              │  (ld)      │\n");
    printf("              └─────┬──────┘\n");
    printf("                    │\n");
    printf("              ┌─────▼──────┐\n");
    printf("              │ a.out      │  (ELF executable)\n");
    printf("              └────────────┘\n\n");

    printf("  GCC actually invokes 'collect2' which wraps 'ld'.\n");
    printf("  See the full command: gcc -v main.c helper.c\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Symbol Resolution
 * ════════════════════════════════════════════════════════════════════ */
static void demo_symbol_resolution(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Symbol Resolution                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Every .o file has:\n");
    printf("  - Defined symbols (functions/variables it provides)\n");
    printf("  - Undefined symbols (functions/variables it needs)\n\n");

    printf("The linker matches undefined → defined:\n\n");

    printf("  main.o defines:  main\n");
    printf("  main.o needs:    printf, compute\n");
    printf("  math.o defines:  compute\n");
    printf("  math.o needs:    sqrt (from libm)\n");
    printf("  libc.so defines: printf\n");
    printf("  libm.so defines: sqrt\n\n");

    printf("Strong vs Weak symbols:\n");
    printf("  ┌───────────────┬──────────────────────────────────────┐\n");
    printf("  │ Type          │ Rule                                 │\n");
    printf("  ├───────────────┼──────────────────────────────────────┤\n");
    printf("  │ Strong+Strong │ ERROR: multiple definition           │\n");
    printf("  │ Strong+Weak   │ Strong wins (weak overridden)        │\n");
    printf("  │ Weak+Weak     │ Linker picks one (arbitrary)         │\n");
    printf("  └───────────────┴──────────────────────────────────────┘\n\n");

    printf("  Functions and initialised globals are strong by default.\n");
    printf("  __attribute__((weak)) makes a symbol weak.\n\n");

    printf("  Demo: global_counter (strong) = %d\n", global_counter);
    printf("  Demo: default_value  (weak)   = %d\n\n", default_value);

    printf("  Undefined symbol errors:\n");
    printf("    /usr/bin/ld: undefined reference to 'foo'\n");
    printf("    collect2: error: ld returned 1 exit status\n\n");
    printf("  This means no .o file or library defined 'foo'.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Relocation
 * ════════════════════════════════════════════════════════════════════ */
static void demo_relocation(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Relocation                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("After symbol resolution, the linker knows where every symbol\n");
    printf("will live in the final binary. It patches all references.\n\n");

    printf("Relocation steps:\n");
    printf("  1. Merge all .text sections into one combined .text.\n");
    printf("  2. Merge all .data sections, all .bss, etc.\n");
    printf("  3. Assign final virtual addresses to each section.\n");
    printf("  4. Walk the relocation entries, compute final addresses,\n");
    printf("     and patch the instruction bytes.\n\n");

    printf("  Example:\n");
    printf("    main.o:  call printf    ← address = 0x00000000 (placeholder)\n");
    printf("    relocation entry: offset=0x15, type=PLT32, symbol=printf\n\n");
    printf("    After linking:\n");
    printf("    a.out:   call 0x401030  ← printf's PLT entry address\n\n");

    printf("  Section merging:\n");
    printf("  ┌──────────────┐   ┌──────────────┐\n");
    printf("  │ main.o .text │   │ math.o .text │\n");
    printf("  └──────┬───────┘   └──────┬───────┘\n");
    printf("         │                  │\n");
    printf("         └──────┬───────────┘\n");
    printf("                │\n");
    printf("  ┌─────────────▼──────────────┐\n");
    printf("  │ a.out .text                │  (all code merged)\n");
    printf("  │  [main.o code][math.o code]│\n");
    printf("  └────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Static Linking
 * ════════════════════════════════════════════════════════════════════ */
static void demo_static_linking(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Static Linking                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Static linking copies needed library code into the executable.\n\n");

    printf("  Static archive (.a) = collection of .o files:\n");
    printf("    ar rcs libmath.a add.o sub.o mul.o\n\n");

    printf("  The linker only pulls in .o members that resolve an\n");
    printf("  undefined symbol — not the entire archive.\n\n");

    printf("  Compile statically:\n");
    printf("    gcc -static main.c -lm -o main_static\n\n");

    printf("  Pros:\n");
    printf("    + Self-contained binary — no runtime dependencies.\n");
    printf("    + No DLL-hell / library version issues.\n");
    printf("    + Slightly faster startup (no dynamic loading).\n\n");

    printf("  Cons:\n");
    printf("    - Larger executable (includes all library code).\n");
    printf("    - No shared memory between processes.\n");
    printf("    - Must recompile to update library code.\n\n");

    printf("  Example sizes (approx):\n");
    printf("    gcc hello.c -o hello          →  ~16 KB\n");
    printf("    gcc -static hello.c -o hello  →  ~900 KB\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Dynamic Linking
 * ════════════════════════════════════════════════════════════════════ */
static void demo_dynamic_linking(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Dynamic Linking                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Dynamic linking defers symbol resolution to load time or\n");
    printf("even call time (lazy binding).\n\n");

    printf("  Shared library (.so) = shared object:\n");
    printf("    gcc -shared -fPIC -o libmath.so add.c sub.c mul.c\n\n");

    printf("  The executable records a NEEDED entry:\n");
    printf("    readelf -d a.out | grep NEEDED\n");
    printf("    → (NEEDED) Shared library: [libc.so.6]\n\n");

    printf("  At load time, ld-linux.so (the dynamic linker):\n");
    printf("    1. Finds the .so files (via LD_LIBRARY_PATH, rpath, etc.)\n");
    printf("    2. Maps them into the process address space.\n");
    printf("    3. Resolves symbols via PLT/GOT mechanism.\n\n");

    printf("  PLT/GOT (lazy binding):\n");
    printf("    First call to printf():\n");
    printf("      1. Jump to PLT stub → jumps to GOT entry.\n");
    printf("      2. GOT initially points to resolver stub.\n");
    printf("      3. Resolver asks ld.so to find printf's real address.\n");
    printf("      4. GOT entry updated to printf's real address.\n");
    printf("      5. Jump to printf.\n");
    printf("    Subsequent calls:\n");
    printf("      1. Jump to PLT stub → GOT now has real address → direct.\n\n");

    printf("  Pros:\n");
    printf("    + Smaller executables.\n");
    printf("    + Shared memory between processes (code pages).\n");
    printf("    + Update library without recompiling programs.\n\n");

    printf("  Cons:\n");
    printf("    - Slightly slower startup (symbol resolution).\n");
    printf("    - Library must be present at runtime.\n");
    printf("    - Version compatibility issues.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Linker Scripts
 * ════════════════════════════════════════════════════════════════════ */
static void demo_linker_scripts(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Linker Scripts                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("A linker script (.ld) controls memory layout and section placement.\n\n");

    printf("  Default script: ld --verbose | less\n\n");

    printf("  Custom linker script example (embedded systems):\n");
    printf("  ┌────────────────────────────────────────────┐\n");
    printf("  │ MEMORY {                                   │\n");
    printf("  │   FLASH (rx)  : ORIGIN=0x08000000, LEN=1M │\n");
    printf("  │   RAM   (rwx) : ORIGIN=0x20000000, LEN=128K│\n");
    printf("  │ }                                          │\n");
    printf("  │ SECTIONS {                                 │\n");
    printf("  │   .text : { *(.text) } > FLASH             │\n");
    printf("  │   .data : { *(.data) } > RAM AT> FLASH     │\n");
    printf("  │   .bss  : { *(.bss)  } > RAM               │\n");
    printf("  │ }                                          │\n");
    printf("  └────────────────────────────────────────────┘\n\n");

    printf("  Use custom script: gcc -T my_linker.ld ...\n\n");

    printf("  When you need custom linker scripts:\n");
    printf("    - Embedded systems (specific memory map)\n");
    printf("    - OS kernel development\n");
    printf("    - Custom section placement\n");
    printf("    - Controlling symbol visibility\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Common Linker Errors
 * ════════════════════════════════════════════════════════════════════ */
static void demo_linker_errors(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Common Linker Errors                       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌─────────────────────────────┬──────────────────────────────┐\n");
    printf("  │ Error                       │ Cause                        │\n");
    printf("  ├─────────────────────────────┼──────────────────────────────┤\n");
    printf("  │ undefined reference to 'X'  │ No .o or .a defines X       │\n");
    printf("  │ multiple definition of 'X'  │ Two .o files define X       │\n");
    printf("  │ cannot find -lX             │ libX.so/libX.a not found    │\n");
    printf("  │ DSO missing from command    │ Shared lib dependency gap    │\n");
    printf("  └─────────────────────────────┴──────────────────────────────┘\n\n");

    printf("  Tips:\n");
    printf("    - Order matters: put libraries AFTER object files.\n");
    printf("      gcc main.c -lm  ✓\n");
    printf("      gcc -lm main.c  ✗ (may fail)\n\n");
    printf("    - Use -Wl,--start-group ... -Wl,--end-group for\n");
    printf("      circular dependencies between static libs.\n\n");
    printf("    - Use -Wl,--trace to see which files the linker processes.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Linker Map File
 * ════════════════════════════════════════════════════════════════════ */
static void demo_linker_map(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Useful Linker Commands                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Generate a linker map:                                 ║\n");
    printf("║  gcc -Wl,-Map=output.map linker.c -o linker            ║\n");
    printf("║  cat output.map                                        ║\n");
    printf("║                                                         ║\n");
    printf("║  See linker trace:                                      ║\n");
    printf("║  gcc -Wl,--trace linker.c -o linker                    ║\n");
    printf("║                                                         ║\n");
    printf("║  See default linker script:                             ║\n");
    printf("║  ld --verbose                                           ║\n");
    printf("║                                                         ║\n");
    printf("║  List shared library dependencies:                      ║\n");
    printf("║  ldd ./linker                                           ║\n");
    printf("║                                                         ║\n");
    printf("║  List symbols after linking:                            ║\n");
    printf("║  nm ./linker                                            ║\n");
    printf("║                                                         ║\n");
    printf("║  See dynamic section:                                   ║\n");
    printf("║  readelf -d ./linker                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 25 — The Linker                                    ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_linker_overview();
    demo_symbol_resolution();
    demo_relocation();
    demo_static_linking();
    demo_dynamic_linking();
    demo_linker_scripts();
    demo_linker_errors();
    demo_linker_map();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 25 — The Linker\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
