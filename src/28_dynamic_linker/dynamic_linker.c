/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 28 — Dynamic Linker (ld-linux.so)                      ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: ld.so, PLT/GOT, lazy binding, symbol lookup            ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>     /* dlopen, dlsym, dlclose */

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What the Dynamic Linker Does
 * ════════════════════════════════════════════════════════════════════ */
static void demo_dynlinker_overview(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What the Dynamic Linker Does               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The dynamic linker (ld-linux.so.2 / ld-linux-x86-64.so.2)\n");
    printf("runs before main(). Its job:\n\n");

    printf("  1. Read the executable's .dynamic section → list of NEEDED libs.\n");
    printf("  2. Search for each shared library (.so) file.\n");
    printf("  3. Load each .so into the process address space (mmap).\n");
    printf("  4. Recursively load dependencies of each .so.\n");
    printf("  5. Perform relocations — patch addresses in code/data.\n");
    printf("  6. Run .init / .init_array constructors.\n");
    printf("  7. Transfer control to the program's _start → main().\n\n");

    printf("  Search order for shared libraries:\n");
    printf("  ┌───┬──────────────────────────────────────────────────┐\n");
    printf("  │ # │ Source                                           │\n");
    printf("  ├───┼──────────────────────────────────────────────────┤\n");
    printf("  │ 1 │ DT_RPATH  (embedded in binary, deprecated)      │\n");
    printf("  │ 2 │ LD_LIBRARY_PATH environment variable             │\n");
    printf("  │ 3 │ DT_RUNPATH (embedded in binary)                  │\n");
    printf("  │ 4 │ ldconfig cache (/etc/ld.so.cache)                │\n");
    printf("  │ 5 │ Default dirs: /lib, /usr/lib                     │\n");
    printf("  └───┴──────────────────────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — PLT and GOT Mechanism
 * ════════════════════════════════════════════════════════════════════ */
static void demo_plt_got(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — PLT and GOT Mechanism                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("PLT = Procedure Linkage Table (in .text, executable)\n");
    printf("GOT = Global Offset Table     (in .data, writable)\n\n");

    printf("  How a call to printf() works with lazy binding:\n\n");

    printf("  FIRST CALL:\n");
    printf("  ┌──────────────────┐\n");
    printf("  │ call printf@plt  │ ← your code\n");
    printf("  └────────┬─────────┘\n");
    printf("           │\n");
    printf("  ┌────────▼─────────┐\n");
    printf("  │ PLT[printf]:     │\n");
    printf("  │   jmp *GOT[printf]│ ← GOT initially points to next line\n");
    printf("  │   push index     │ ← push printf's relocation index\n");
    printf("  │   jmp PLT[0]    │ ← jump to resolver\n");
    printf("  └────────┬─────────┘\n");
    printf("           │\n");
    printf("  ┌────────▼─────────┐\n");
    printf("  │ PLT[0]:          │\n");
    printf("  │   push GOT[1]   │ ← link_map struct\n");
    printf("  │   jmp *GOT[2]   │ ← _dl_runtime_resolve\n");
    printf("  └────────┬─────────┘\n");
    printf("           │\n");
    printf("  ┌────────▼─────────────┐\n");
    printf("  │ _dl_runtime_resolve:  │\n");
    printf("  │   Find printf in libc │\n");
    printf("  │   Update GOT[printf]  │ ← now points to real printf\n");
    printf("  │   Jump to printf      │\n");
    printf("  └──────────────────────┘\n\n");

    printf("  SUBSEQUENT CALLS:\n");
    printf("  ┌──────────────────┐\n");
    printf("  │ call printf@plt  │\n");
    printf("  └────────┬─────────┘\n");
    printf("           │\n");
    printf("  ┌────────▼─────────┐\n");
    printf("  │ PLT[printf]:     │\n");
    printf("  │   jmp *GOT[printf]│ ← GOT now points to real printf!\n");
    printf("  └────────┬─────────┘\n");
    printf("           │\n");
    printf("  ┌────────▼─────────┐\n");
    printf("  │ printf() in libc │ ← direct, no resolver overhead\n");
    printf("  └──────────────────┘\n\n");

    printf("  To disable lazy binding (resolve all at startup):\n");
    printf("    LD_BIND_NOW=1 ./program\n");
    printf("  Or link with: gcc -Wl,-z,now ...\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Position Independent Code (PIC)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_pic(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Position Independent Code (PIC)            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Shared libraries must work at any address → PIC.\n\n");

    printf("  Without PIC (position-dependent):\n");
    printf("    mov eax, [0x601020]     ← absolute address, fixed!\n\n");

    printf("  With PIC:\n");
    printf("    mov eax, [rip + offset] ← PC-relative, works anywhere\n\n");

    printf("  For global variables accessed by shared libraries:\n");
    printf("    The GOT holds the actual addresses.\n");
    printf("    Code accesses globals via GOT[symbol] indirection.\n\n");

    printf("  Compile flags:\n");
    printf("    gcc -fPIC -shared -o libfoo.so foo.c\n\n");

    printf("  PIE (Position Independent Executable):\n");
    printf("    gcc -fPIE -pie -o program main.c\n");
    printf("    (Default on modern GCC — enables ASLR.)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — dlopen / dlsym (Runtime Loading)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_dlopen(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — dlopen / dlsym (Runtime Loading)           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Programs can load shared libraries at runtime:\n\n");

    printf("  void *handle = dlopen(\"libm.so.6\", RTLD_LAZY);\n");
    printf("  double (*cosine)(double) = dlsym(handle, \"cos\");\n");
    printf("  printf(\"cos(0) = %%f\\n\", cosine(0.0));\n");
    printf("  dlclose(handle);\n\n");

    /* Live demo with libm */
    void *handle = dlopen("libm.so.6", RTLD_LAZY);
    if (handle) {
        double (*cosine)(double) = (double(*)(double))dlsym(handle, "cos");
        double (*sine)(double)   = (double(*)(double))dlsym(handle, "sin");
        if (cosine && sine) {
            printf("  Live demo with dlopen(\"libm.so.6\"):\n");
            printf("    cos(0.0) = %f\n", cosine(0.0));
            printf("    sin(1.5708) = %f  (approx pi/2)\n", sine(1.5708));
        }
        dlclose(handle);
    } else {
        printf("  (dlopen demo skipped: %s)\n", dlerror());
    }

    printf("\n  dlopen flags:\n");
    printf("    RTLD_LAZY   — resolve symbols on first use\n");
    printf("    RTLD_NOW    — resolve all symbols immediately\n");
    printf("    RTLD_GLOBAL — symbols available to other dlopen'd libs\n");
    printf("    RTLD_LOCAL  — symbols private to this handle\n\n");

    printf("  Error checking: dlerror() returns last error string.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Constructor/Destructor Functions
 * ════════════════════════════════════════════════════════════════════ */

/* These run before/after main() via .init_array/.fini_array */
__attribute__((constructor))
static void my_constructor(void)
{
    /* Runs before main — output suppressed to keep demo clean.
     * Uncomment the printf to see it in action. */
    /* printf("  [constructor] Running before main!\n"); */
}

__attribute__((destructor))
static void my_destructor(void)
{
    /* Runs after main returns. */
    /* printf("  [destructor] Running after main!\n"); */
}

static void demo_constructors(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Constructor/Destructor Functions            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The dynamic linker calls .init_array functions before main().\n");
    printf("After main() returns, .fini_array functions run.\n\n");

    printf("  __attribute__((constructor))\n");
    printf("  void setup(void) { /* runs before main */ }\n\n");
    printf("  __attribute__((destructor))\n");
    printf("  void cleanup(void) { /* runs after main */ }\n\n");

    printf("  Order:\n");
    printf("    1. ld.so resolves symbols, performs relocations.\n");
    printf("    2. .preinit_array functions (main executable only).\n");
    printf("    3. .init_array functions (each .so, then main).\n");
    printf("    4. main() runs.\n");
    printf("    5. .fini_array functions (reverse order).\n\n");

    printf("  Shared libraries can also have constructors.\n");
    printf("  This is how library initialisation works (e.g., OpenSSL).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Debugging ld.so
 * ════════════════════════════════════════════════════════════════════ */
static void demo_debugging(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Debugging the Dynamic Linker               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  List needed shared libraries:                          ║\n");
    printf("║  ldd ./dynamic_linker                                   ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace library loading:                                 ║\n");
    printf("║  LD_DEBUG=libs ./dynamic_linker                         ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace symbol resolution:                               ║\n");
    printf("║  LD_DEBUG=symbols ./dynamic_linker                      ║\n");
    printf("║                                                         ║\n");
    printf("║  Trace bindings (PLT/GOT):                              ║\n");
    printf("║  LD_DEBUG=bindings ./dynamic_linker                     ║\n");
    printf("║                                                         ║\n");
    printf("║  All debug categories:                                  ║\n");
    printf("║  LD_DEBUG=all ./dynamic_linker 2>&1 | head -50          ║\n");
    printf("║                                                         ║\n");
    printf("║  Preload a library (override symbols):                  ║\n");
    printf("║  LD_PRELOAD=./libhook.so ./dynamic_linker               ║\n");
    printf("║                                                         ║\n");
    printf("║  View PLT/GOT entries:                                  ║\n");
    printf("║  objdump -d -j .plt ./dynamic_linker                    ║\n");
    printf("║  readelf -r ./dynamic_linker                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 28 — Dynamic Linker (ld-linux.so)                  ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_dynlinker_overview();
    demo_plt_got();
    demo_pic();
    demo_dlopen();
    demo_constructors();
    demo_debugging();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 28 — Dynamic Linker\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
