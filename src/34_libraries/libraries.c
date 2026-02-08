/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 34 — Static & Dynamic Libraries                        ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: .a archives, .so shared objects, linking, dlopen       ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Example "library" functions ─────────────────────────────── */

int lib_add(int a, int b)      { return a + b; }
int lib_multiply(int a, int b) { return a * b; }
int lib_square(int x)          { return x * x; }

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Why Libraries?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_why_libraries(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Why Libraries?                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Libraries package reusable code for sharing across projects.\n\n");

    printf("  Benefits:\n");
    printf("    - Code reuse (don't reinvent the wheel)\n");
    printf("    - Separate compilation (faster builds)\n");
    printf("    - Encapsulation (implementation hidden)\n");
    printf("    - Versioning (update library without recompiling users)\n\n");

    printf("  Two types:\n");
    printf("  ┌────────────────────┬──────────────────────────────────┐\n");
    printf("  │ Type               │ Extension / Characteristics      │\n");
    printf("  ├────────────────────┼──────────────────────────────────┤\n");
    printf("  │ Static library     │ .a  — copied into the executable │\n");
    printf("  │ Dynamic/Shared lib │ .so — loaded at runtime          │\n");
    printf("  └────────────────────┴──────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Creating a Static Library (.a)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_static_library(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Creating a Static Library (.a)             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Step 1: Compile source files to object files:\n");
    printf("    gcc -c add.c -o add.o\n");
    printf("    gcc -c multiply.c -o multiply.o\n\n");

    printf("  Step 2: Archive into a static library:\n");
    printf("    ar rcs libmath.a add.o multiply.o\n\n");

    printf("  ar flags:\n");
    printf("    r = insert/replace files\n");
    printf("    c = create archive if not exists\n");
    printf("    s = write index (ranlib equivalent)\n\n");

    printf("  Step 3: Use the library:\n");
    printf("    gcc main.c -L. -lmath -o program\n");
    printf("    # -L. = look in current dir for libraries\n");
    printf("    # -lmath = link with libmath.a\n\n");

    printf("  Inspect the archive:\n");
    printf("    ar t libmath.a         ← list contents\n");
    printf("    ar x libmath.a add.o   ← extract a member\n");
    printf("    nm libmath.a           ← list all symbols\n\n");

    printf("  How it works:\n");
    printf("    The linker searches the .a file and extracts ONLY the\n");
    printf("    .o members that resolve undefined symbols. Unused .o\n");
    printf("    files are NOT included in the executable.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Creating a Shared Library (.so)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_shared_library(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Creating a Shared Library (.so)            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Step 1: Compile with Position Independent Code:\n");
    printf("    gcc -fPIC -c add.c -o add.o\n");
    printf("    gcc -fPIC -c multiply.c -o multiply.o\n\n");

    printf("  Step 2: Link into a shared object:\n");
    printf("    gcc -shared -o libmath.so add.o multiply.o\n\n");

    printf("  Step 3: Use the library:\n");
    printf("    gcc main.c -L. -lmath -o program\n\n");

    printf("  Step 4: Runtime — the .so must be findable:\n");
    printf("    Option A: LD_LIBRARY_PATH=. ./program\n");
    printf("    Option B: Install to /usr/lib and run ldconfig\n");
    printf("    Option C: gcc -Wl,-rpath,/path/to/lib ...\n\n");

    printf("  Versioning convention:\n");
    printf("    libmath.so         → symlink to latest\n");
    printf("    libmath.so.1       → symlink to soname\n");
    printf("    libmath.so.1.0.0   → actual file\n\n");

    printf("  Set soname:\n");
    printf("    gcc -shared -Wl,-soname,libmath.so.1 -o libmath.so.1.0.0\n");
    printf("    ln -s libmath.so.1.0.0 libmath.so.1\n");
    printf("    ln -s libmath.so.1 libmath.so\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Static vs Dynamic: Comparison
 * ════════════════════════════════════════════════════════════════════ */
static void demo_comparison(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Static vs Dynamic Comparison               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌──────────────────────┬────────────────┬─────────────────┐\n");
    printf("  │ Aspect               │ Static (.a)    │ Dynamic (.so)   │\n");
    printf("  ├──────────────────────┼────────────────┼─────────────────┤\n");
    printf("  │ Executable size      │ Larger         │ Smaller          │\n");
    printf("  │ Runtime dependency   │ None           │ .so must exist   │\n");
    printf("  │ Memory sharing       │ No             │ Yes (shared pages)│\n");
    printf("  │ Update without rebuild│ No            │ Yes              │\n");
    printf("  │ Startup speed        │ Faster         │ Slightly slower  │\n");
    printf("  │ Portability          │ More portable  │ Less portable    │\n");
    printf("  │ Symbol visibility    │ All or nothing │ Controllable     │\n");
    printf("  │ When to use          │ Embedded, deploy│ Desktop, servers│\n");
    printf("  └──────────────────────┴────────────────┴─────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Symbol Visibility
 * ════════════════════════════════════════════════════════════════════ */
static void demo_visibility(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Symbol Visibility                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  By default, all functions in a .so are visible (exported).\n");
    printf("  Best practice: hide internal symbols.\n\n");

    printf("  Method 1 — Visibility attributes:\n");
    printf("    __attribute__((visibility(\"default\")))  ← exported\n");
    printf("    __attribute__((visibility(\"hidden\")))   ← internal only\n\n");

    printf("  Method 2 — Compile with hidden default:\n");
    printf("    gcc -fvisibility=hidden -fPIC -shared ...\n");
    printf("    Then mark exports explicitly:\n");
    printf("    #define EXPORT __attribute__((visibility(\"default\")))\n");
    printf("    EXPORT int public_func(int x) { ... }\n\n");

    printf("  Method 3 — Version script:\n");
    printf("    Create libmath.map:\n");
    printf("      { global: lib_add; lib_multiply; local: *; };\n");
    printf("    gcc -shared -Wl,--version-script=libmath.map ...\n\n");

    printf("  Benefits of hiding symbols:\n");
    printf("    - Faster dynamic linking (fewer symbols to resolve)\n");
    printf("    - Smaller .dynsym table\n");
    printf("    - Better optimisation (compiler knows calls are local)\n");
    printf("    - Prevents symbol conflicts between libraries\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — pkg-config
 * ════════════════════════════════════════════════════════════════════ */
static void demo_pkgconfig(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — pkg-config                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  pkg-config helps find compiler/linker flags for libraries.\n\n");

    printf("  Usage:\n");
    printf("    pkg-config --cflags libpng    → -I/usr/include/libpng16\n");
    printf("    pkg-config --libs libpng      → -lpng16 -lz\n");
    printf("    gcc $(pkg-config --cflags --libs libpng) main.c\n\n");

    printf("  A .pc file (e.g., /usr/lib/pkgconfig/libmath.pc):\n");
    printf("    prefix=/usr/local\n");
    printf("    libdir=${prefix}/lib\n");
    printf("    includedir=${prefix}/include\n");
    printf("    Name: libmath\n");
    printf("    Description: Math utility library\n");
    printf("    Version: 1.0.0\n");
    printf("    Libs: -L${libdir} -lmath\n");
    printf("    Cflags: -I${includedir}\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Demo
 * ════════════════════════════════════════════════════════════════════ */
static void demo_live(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Live Demo                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Using our 'library' functions:\n");
    printf("    lib_add(10, 20)      = %d\n", lib_add(10, 20));
    printf("    lib_multiply(6, 7)   = %d\n", lib_multiply(6, 7));
    printf("    lib_square(9)        = %d\n\n", lib_square(9));

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Create static library from this file:                  ║\n");
    printf("║  gcc -c libraries.c -o libraries.o                      ║\n");
    printf("║  ar rcs libdemo.a libraries.o                           ║\n");
    printf("║  ar t libdemo.a                                         ║\n");
    printf("║  nm libdemo.a                                           ║\n");
    printf("║                                                         ║\n");
    printf("║  Create shared library:                                 ║\n");
    printf("║  gcc -fPIC -c libraries.c -o libraries_pic.o            ║\n");
    printf("║  gcc -shared -o libdemo.so libraries_pic.o              ║\n");
    printf("║  ldd libdemo.so                                         ║\n");
    printf("║  nm -D libdemo.so                                       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 34 — Static & Dynamic Libraries                    ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_why_libraries();
    demo_static_library();
    demo_shared_library();
    demo_comparison();
    demo_visibility();
    demo_pkgconfig();
    demo_live();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 34 — Static & Dynamic Libraries\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
