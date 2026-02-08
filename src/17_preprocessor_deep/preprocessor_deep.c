/*
 * Chapter 17 — Preprocessor In-Depth
 *
 * A comprehensive tour of the C preprocessor:
 *   1. #include resolution (system vs local, search paths)
 *   2. Object-like & function-like macros (and pitfalls)
 *   3. Stringification (#) and token pasting (##)
 *   4. Variadic macros (__VA_ARGS__)
 *   5. Conditional compilation (#if/#ifdef/#ifndef/#elif/#else/#endif)
 *   6. Predefined macros (__FILE__, __LINE__, __func__, etc.)
 *   7. #pragma and _Pragma
 *   8. Include guards vs #pragma once
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/17_preprocessor_deep \
 *            src/17_preprocessor_deep/preprocessor_deep.c
 * Run:   ./bin/17_preprocessor_deep
 *
 * Try:
 *   gcc -E  src/17_preprocessor_deep/preprocessor_deep.c | tail -80
 *   gcc -dM -E src/17_preprocessor_deep/preprocessor_deep.c | sort
 *   gcc -E -dD src/17_preprocessor_deep/preprocessor_deep.c | head -100
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: #include Resolution
 * ════════════════════════════════════════════════════════════════ */

/*
 * Two forms:
 *   #include <header.h>   — searches system include paths
 *   #include "header.h"   — searches local directory first, then system
 *
 * The search order for "header.h":
 *   1. Directory of the file containing the #include
 *   2. Directories specified by -iquote
 *   3. Directories specified by -I
 *   4. System directories (e.g., /usr/include)
 *
 * For <header.h>:
 *   1. Directories specified by -I
 *   2. System directories
 *
 * Use: gcc -v -E file.c   to see the actual search paths.
 */

static void demo_include_resolution(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: #include Resolution                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("Two include forms:\n");
    printf("  #include <stdio.h>   → system paths only\n");
    printf("  #include \"myfile.h\"  → local dir first, then system\n\n");

    printf("Search order for #include \"header.h\":\n");
    printf("  1. Directory of the current source file\n");
    printf("  2. -iquote directories\n");
    printf("  3. -I directories\n");
    printf("  4. System directories (/usr/include, etc.)\n\n");

    printf("Search order for #include <header.h>:\n");
    printf("  1. -I directories\n");
    printf("  2. System directories\n\n");

    printf("Try: gcc -v -E %s 2>&1 | grep 'search starts'\n", __FILE__);
    printf("     (reveals the exact search path list)\n\n");

    printf("Try: echo '#include <stdio.h>' | gcc -E -H -x c - 2>&1 | head -5\n");
    printf("     (-H flag shows each included file with nesting depth)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Object-like & Function-like Macros
 * ════════════════════════════════════════════════════════════════ */

/* Object-like macros — simple text substitution */
#define PI              3.14159265358979
#define MAX_BUFFER      1024
#define AUTHOR          "Modular-C-Demos"

/* Function-like macros — beware the pitfalls! */

/* PITFALL: no parentheses around parameter */
#define SQUARE_BAD(x)   x * x
/* CORRECT: parenthesised */
#define SQUARE_GOOD(x)  ((x) * (x))

/* PITFALL: double evaluation */
#define MAX_BAD(a, b)   ((a) > (b) ? (a) : (b))
/* The above evaluates a or b twice — dangerous with side effects */

/* Multi-statement macro with do-while(0) idiom */
#define SWAP_INT(a, b)  do { int _tmp = (a); (a) = (b); (b) = _tmp; } while(0)

static void demo_macros(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Object-like & Function-like Macros     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Object-like macros ──\n");
    printf("  PI         = %f\n", PI);
    printf("  MAX_BUFFER = %d\n", MAX_BUFFER);
    printf("  AUTHOR     = \"%s\"\n\n", AUTHOR);

    printf("── Function-like macro pitfall: SQUARE ──\n");
    printf("  #define SQUARE_BAD(x)  x * x\n");
    printf("  #define SQUARE_GOOD(x) ((x) * (x))\n\n");

    int val = 5;
    printf("  SQUARE_BAD(5)       = %d  (OK)\n", SQUARE_BAD(5));
    printf("  SQUARE_GOOD(5)      = %d  (OK)\n", SQUARE_GOOD(5));
    printf("  SQUARE_BAD(2+3)     = %d  (WRONG! expands to 2+3*2+3 = 11)\n",
           SQUARE_BAD(2+3));
    printf("  SQUARE_GOOD(2+3)    = %d  (CORRECT! expands to ((2+3)*(2+3)))\n\n",
           SQUARE_GOOD(2+3));

    printf("── Double-evaluation pitfall: MAX ──\n");
    printf("  #define MAX_BAD(a,b) ((a)>(b)?(a):(b))\n");
    int a = 3, b = 5;
    printf("  MAX_BAD(%d, %d)     = %d  (OK here, but...)\n", a, b, MAX_BAD(a, b));
    int x = 3, y = 5;
    int result = MAX_BAD(x++, y++);
    printf("  MAX_BAD(x++, y++) with x=3, y=5:\n");
    printf("    result=%d  x=%d  y=%d  (y got incremented TWICE!)\n\n", result, x, y);
    (void)val;

    printf("── SWAP using do { ... } while(0) ──\n");
    int p = 10, q = 20;
    printf("  Before: p=%d  q=%d\n", p, q);
    SWAP_INT(p, q);
    printf("  After:  p=%d  q=%d\n\n", p, q);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Stringification (#) and Token Pasting (##)
 * ════════════════════════════════════════════════════════════════ */

/* # turns macro argument into a string literal */
#define STRINGIFY(x)        #x
#define TOSTRING(x)         STRINGIFY(x)    /* indirection for expansion */

/* ## pastes two tokens together */
#define CONCAT(a, b)        a##b
#define MAKE_VAR(prefix, n) prefix##_##n

/* Practical use: assert-like macro */
#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            printf("  CHECK FAILED: %s  [%s:%d]\n", #expr, __FILE__, __LINE__); \
        } else { \
            printf("  CHECK PASSED: %s\n", #expr); \
        } \
    } while(0)

static void demo_stringify_paste(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Stringification (#) & Token Pasting (##)║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Stringification with # ──\n");
    printf("  #define STRINGIFY(x) #x\n");
    printf("  STRINGIFY(hello world)  = \"%s\"\n", STRINGIFY(hello world));
    printf("  STRINGIFY(3 + 4)        = \"%s\"\n\n", STRINGIFY(3 + 4));

    printf("── Why two levels? ──\n");
    printf("  #define TOSTRING(x) STRINGIFY(x)\n");
    printf("  STRINGIFY(MAX_BUFFER) = \"%s\"  (literal text)\n", STRINGIFY(MAX_BUFFER));
    printf("  TOSTRING(MAX_BUFFER)  = \"%s\"   (expanded first!)\n\n", TOSTRING(MAX_BUFFER));

    printf("── Token pasting with ## ──\n");
    printf("  #define CONCAT(a,b) a##b\n");
    int CONCAT(my, Var) = 42;    /* creates: int myVar = 42; */
    printf("  int CONCAT(my, Var) = 42;  → myVar = %d\n\n", myVar);

    int MAKE_VAR(sensor, 0) = 100;
    int MAKE_VAR(sensor, 1) = 200;
    printf("  MAKE_VAR(sensor, 0)  → sensor_0 = %d\n", sensor_0);
    printf("  MAKE_VAR(sensor, 1)  → sensor_1 = %d\n\n", sensor_1);

    printf("── Practical: CHECK macro (assert-like) ──\n");
    CHECK(1 + 1 == 2);
    CHECK(PI > 3.0);
    CHECK(42 == 0);
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Variadic Macros (__VA_ARGS__)
 * ════════════════════════════════════════════════════════════════ */

/* Basic variadic macro */
#define LOG(fmt, ...) \
    printf("  [LOG] " fmt "\n", __VA_ARGS__)

/* GNU extension: ##__VA_ARGS__ swallows the comma if empty */
#define LOG_SAFE(fmt, ...) \
    printf("  [LOG] " fmt "\n", ##__VA_ARGS__)

/* Debug macro that includes file and line */
#define DBG(fmt, ...) \
    printf("  [DBG %s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* Macro that counts its arguments (up to 5) */
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

static void demo_variadic_macros(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Variadic Macros (__VA_ARGS__)          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Basic variadic macro ──\n");
    printf("  #define LOG(fmt, ...) printf(\"[LOG] \" fmt, __VA_ARGS__)\n\n");
    LOG("Value of x: %d", 42);
    LOG("Name: %s, Age: %d", "Alice", 30);

    printf("\n── Safe variadic (GNU ##__VA_ARGS__) ──\n");
    printf("  #define LOG_SAFE(fmt, ...) printf(fmt, ##__VA_ARGS__)\n");
    printf("  (##__VA_ARGS__ removes trailing comma if no args)\n\n");
    LOG_SAFE("No extra args needed here");
    LOG_SAFE("With arg: %d", 99);

    printf("\n── Debug macro with location ──\n");
    DBG("entering function");
    DBG("x = %d, y = %d", 10, 20);

    printf("\n── Counting arguments (trick) ──\n");
    printf("  COUNT_ARGS(a)          = %d\n", COUNT_ARGS(a));
    printf("  COUNT_ARGS(a, b)       = %d\n", COUNT_ARGS(a, b));
    printf("  COUNT_ARGS(a, b, c, d) = %d\n\n", COUNT_ARGS(a, b, c, d));
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Conditional Compilation
 * ════════════════════════════════════════════════════════════════ */

/* Define some feature flags for demonstration */
#define FEATURE_LOGGING  1
#define FEATURE_METRICS  0
/* FEATURE_TRACING intentionally NOT defined */

/* Platform detection (common pattern) */
#if defined(__linux__)
    #define PLATFORM_NAME "Linux"
#elif defined(__APPLE__)
    #define PLATFORM_NAME "macOS"
#elif defined(_WIN32)
    #define PLATFORM_NAME "Windows"
#else
    #define PLATFORM_NAME "Unknown"
#endif

/* Version comparison */
#define APP_VERSION 204
#if APP_VERSION >= 200
    #define HAS_NEW_API 1
#else
    #define HAS_NEW_API 0
#endif

static void demo_conditional_compilation(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Conditional Compilation                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── #ifdef / #ifndef ──\n");
    printf("  FEATURE_LOGGING is defined  → ");
#ifdef FEATURE_LOGGING
    printf("YES, logging enabled\n");
#else
    printf("NO, logging disabled\n");
#endif

    printf("  FEATURE_TRACING is defined  → ");
#ifdef FEATURE_TRACING
    printf("YES, tracing enabled\n");
#else
    printf("NO, tracing not defined\n");
#endif

    printf("\n── #if with expressions ──\n");
    printf("  FEATURE_METRICS = %d → ", FEATURE_METRICS);
#if FEATURE_METRICS
    printf("metrics ON\n");
#else
    printf("metrics OFF (value is 0)\n");
#endif

    printf("\n── #if / #elif chain (platform detection) ──\n");
    printf("  Detected platform: %s\n", PLATFORM_NAME);

    printf("\n── Version comparison ──\n");
    printf("  APP_VERSION = %d\n", APP_VERSION);
#if HAS_NEW_API
    printf("  New API is available (version >= 200)\n");
#else
    printf("  Using legacy API (version < 200)\n");
#endif

    printf("\n── Compile-time enable/disable ──\n");
    printf("  Try: gcc -DFEATURE_TRACING %s\n", __FILE__);
    printf("       (now FEATURE_TRACING will be defined)\n");
    printf("  Try: gcc -UFEATURE_LOGGING %s\n", __FILE__);
    printf("       (-U undefines a macro)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Predefined Macros
 * ════════════════════════════════════════════════════════════════ */

static void demo_predefined_macros(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Predefined Macros                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Standard predefined macros ──\n");
    printf("  __FILE__          = \"%s\"\n", __FILE__);
    printf("  __LINE__          = %d\n", __LINE__);
    printf("  __DATE__          = \"%s\"\n", __DATE__);
    printf("  __TIME__          = \"%s\"\n", __TIME__);
    printf("  __func__          = \"%s\"\n", __func__);
    printf("  __STDC__          = %d\n", __STDC__);
    printf("  __STDC_VERSION__  = %ldL\n\n", __STDC_VERSION__);

    printf("  Meanings:\n");
    printf("    __STDC__ = 1           → compiler conforms to ISO C\n");
    printf("    __STDC_VERSION__:\n");
    printf("      199901L = C99\n");
    printf("      201112L = C11\n");
    printf("      201710L = C17\n");
    printf("      202311L = C23\n\n");

#if __STDC_VERSION__ >= 201112L
    printf("  This compiler supports C11 or later.\n\n");
#elif __STDC_VERSION__ >= 199901L
    printf("  This compiler supports C99.\n\n");
#endif

    printf("── Compiler-specific macros ──\n");
#ifdef __GNUC__
    printf("  __GNUC__          = %d\n", __GNUC__);
    printf("  __GNUC_MINOR__    = %d\n", __GNUC_MINOR__);
    printf("  __GNUC_PATCHLEVEL__= %d\n", __GNUC_PATCHLEVEL__);
#endif
#ifdef __clang__
    printf("  __clang__         = %d  (this is Clang!)\n", __clang__);
#endif

    printf("\n  Try: gcc -dM -E /dev/null | sort\n");
    printf("       (lists ALL predefined macros for your compiler)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: #pragma and _Pragma
 * ════════════════════════════════════════════════════════════════ */

/* #pragma controls compiler-specific behaviour */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static void function_with_unused_var(void)
{
    int unused_warning_suppressed = 42;
    /* No warning due to the #pragma above */
}
#pragma GCC diagnostic pop

/* _Pragma is the C99 operator form — can be used in macros */
#define SUPPRESS_WARNING(warn) \
    _Pragma(STRINGIFY(GCC diagnostic ignored warn))

#define BEGIN_NO_WARNINGS \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wall\"")

#define END_NO_WARNINGS \
    _Pragma("GCC diagnostic pop")

static void demo_pragma(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: #pragma and _Pragma                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── #pragma GCC diagnostic ──\n");
    printf("  #pragma GCC diagnostic push\n");
    printf("  #pragma GCC diagnostic ignored \"-Wunused-variable\"\n");
    printf("  ... code with unused var — no warning ...\n");
    printf("  #pragma GCC diagnostic pop\n\n");

    printf("── _Pragma (C99 operator form) ──\n");
    printf("  Equivalent to #pragma, but usable inside macros.\n");
    printf("  _Pragma(\"GCC diagnostic push\")\n");
    printf("  Useful for creating warning-suppression macros:\n\n");
    printf("  #define BEGIN_NO_WARNINGS \\\n");
    printf("      _Pragma(\"GCC diagnostic push\") \\\n");
    printf("      _Pragma(\"GCC diagnostic ignored \\\"-Wall\\\"\")\n\n");

    printf("── Common #pragma uses ──\n");
    printf("  #pragma once          — include guard (non-standard but universal)\n");
    printf("  #pragma pack(1)       — struct packing (no padding)\n");
    printf("  #pragma message(\"...\") — compile-time message\n");
    printf("  #pragma GCC optimize  — per-function optimisation level\n\n");

    function_with_unused_var();
}

/* ════════════════════════════════════════════════════════════════
 *  Section 8: Include Guards vs #pragma once
 * ════════════════════════════════════════════════════════════════ */

static void demo_include_guards(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 8: Include Guards vs #pragma once         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── Traditional include guard ──\n");
    printf("  // myheader.h\n");
    printf("  #ifndef MYHEADER_H\n");
    printf("  #define MYHEADER_H\n");
    printf("  ... declarations ...\n");
    printf("  #endif /* MYHEADER_H */\n\n");

    printf("── #pragma once ──\n");
    printf("  // myheader.h\n");
    printf("  #pragma once\n");
    printf("  ... declarations ...\n\n");

    printf("── Comparison ──\n");
    printf("  ┌───────────────────┬──────────────────┬──────────────────┐\n");
    printf("  │ Feature           │ Include Guard    │ #pragma once     │\n");
    printf("  ├───────────────────┼──────────────────┼──────────────────┤\n");
    printf("  │ Standard C?       │ YES (portable)   │ NO (extension)   │\n");
    printf("  │ Supported?        │ All compilers    │ GCC,Clang,MSVC   │\n");
    printf("  │ Symlinks handled? │ YES              │ May fail         │\n");
    printf("  │ Naming collisions │ Possible         │ Impossible       │\n");
    printf("  │ Verbosity         │ 3 lines          │ 1 line           │\n");
    printf("  └───────────────────┴──────────────────┴──────────────────┘\n\n");

    printf("  Recommendation: Use include guards for maximum portability.\n");
    printf("  Use #pragma once for convenience in GCC/Clang projects.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 9: Macro that changes behaviour based on #ifdef
 * ════════════════════════════════════════════════════════════════ */

/* A logging framework that compiles away in release builds */
#ifdef VERBOSE_MODE
    #define TRACE(fmt, ...) \
        printf("  [TRACE %s:%d %s] " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define TRACE(fmt, ...) ((void)0)
#endif

/* An allocator that can be switched to debug mode */
#ifdef DEBUG_ALLOC
    #define MY_ALLOC(size) debug_alloc(size, __FILE__, __LINE__)
    static void *debug_alloc(size_t size, const char *file, int line) {
        void *p = malloc(size);
        printf("  [ALLOC] %zu bytes at %p  (%s:%d)\n", size, p, file, line);
        return p;
    }
#else
    #define MY_ALLOC(size) malloc(size)
#endif

static void demo_conditional_macro_demo(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 9: Demo — Conditional Macro Behaviour     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("── TRACE macro ──\n");
    printf("  Without -DVERBOSE_MODE: TRACE() compiles to nothing.\n");
    printf("  With    -DVERBOSE_MODE: TRACE() prints file:line:func.\n\n");
    printf("  Calling TRACE(\"hello %%d\", 42):\n");
    TRACE("hello %d", 42);
#ifndef VERBOSE_MODE
    printf("    (silent — VERBOSE_MODE not defined)\n\n");
#endif

    printf("── MY_ALLOC macro ──\n");
    printf("  Without -DDEBUG_ALLOC: MY_ALLOC(n) = malloc(n)\n");
    printf("  With    -DDEBUG_ALLOC: MY_ALLOC(n) = debug_alloc(n,...)\n\n");
    void *ptr = MY_ALLOC(64);
#ifndef DEBUG_ALLOC
    printf("  MY_ALLOC(64) = %p  (plain malloc, no tracking)\n\n", ptr);
#endif
    free(ptr);

    printf("  Try: gcc -DVERBOSE_MODE -DDEBUG_ALLOC -std=c99 %s\n", __FILE__);
    printf("       (enables both debug macros)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main
 * ════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 17 — Preprocessor In-Depth                ║\n");
    printf("║  Everything about #include, #define, #if, and more ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    demo_include_resolution();
    demo_macros();
    demo_stringify_paste();
    demo_variadic_macros();
    demo_conditional_compilation();
    demo_predefined_macros();
    demo_pragma();
    demo_include_guards();
    demo_conditional_macro_demo();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: The preprocessor is a powerful text-transform\n");
    printf(" engine.  Master it to write portable, configurable C.\n");
    printf("════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("  Key commands to explore:\n");
    printf("    gcc -E  file.c              → see preprocessed output\n");
    printf("    gcc -E -dM /dev/null        → list all predefined macros\n");
    printf("    gcc -E -dD file.c           → show #define + expansion\n");
    printf("    gcc -DFLAG -UOTHER file.c   → define/undefine macros\n");
    printf("    gcc -I/path file.c          → add include search path\n\n");

    return 0;
}
