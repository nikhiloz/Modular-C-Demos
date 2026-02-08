/*
 * Chapter 11 — The C Preprocessor
 *
 * The preprocessor runs BEFORE the compiler even sees your code.
 * It operates on text — no type checking, no syntax analysis.
 * This is both powerful and dangerous.
 *
 * Topics covered:
 *   1. Object-like macros (constants via #define)
 *   2. Function-like macros (inline code generation)
 *   3. Stringification (#) and token pasting (##)
 *   4. Variadic macros (__VA_ARGS__)
 *   5. Predefined macros (__FILE__, __LINE__, etc.)
 *   6. Conditional compilation (#if, #ifdef, #ifndef)
 *   7. Macro hygiene (do-while trick, parentheses)
 *   8. X-macros (define data once, expand many ways)
 *   9. Include guards vs #pragma once
 *
 * Build: make 11_preprocessor
 * Run:   ./bin/11_preprocessor
 *
 * Try these:
 *   gcc -E src/11_preprocessor/preprocessor.c | tail -80   # see expanded macros
 *   gcc -DDEBUG src/11_preprocessor/preprocessor.c         # enable DEBUG path
 *   gcc -E -dM src/11_preprocessor/preprocessor.c | sort   # all predefined macros
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════
 *  Object-like Macros
 * ════════════════════════════════════════════════════════════════ */

/* Simple text replacement — no type safety, no scope.
 * Convention: ALL_CAPS so readers know it's a macro, not a variable. */
#define PI          3.14159265358979
#define MAX_SENSORS 64

/* ════════════════════════════════════════════════════════════════
 *  Function-like Macros (with hygiene)
 * ════════════════════════════════════════════════════════════════ */

/* ALWAYS parenthesise every parameter AND the whole expression.
 * Without parens: SQUARE(1+2) expands to 1+2*1+2 = 5, not 9!   */
#define SQUARE(x)   ((x) * (x))

/* Variadic macro — the ## before __VA_ARGS__ handles zero args
 * (GCC/Clang extension; C23 adds __VA_OPT__ as the standard way) */
#define DEBUG_LOG(fmt, ...) \
    printf("[DEBUG %s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* do { ... } while(0) trick: makes the macro behave like a single
 * statement.  Without it, if(cond) SWAP_INT(a,b); else ... breaks
 * because the braces create a compound statement, not one stmt.   */
#define SWAP_INT(a, b) do { \
    int _tmp = (a);         \
    (a) = (b);              \
    (b) = _tmp;             \
} while (0)

/* Stringification (#) turns a token into a string literal.
 * Great for debug printing — the variable name appears in output. */
#define PRINT_VAR(x) printf("  " #x " = %d\n", (x))

/* Token pasting (##) glues two tokens into one identifier.
 * Use case: generating families of related functions or variables. */
#define MAKE_GETTER(field) \
    static int get_##field(void) { return config.field; }

/* ════════════════════════════════════════════════════════════════
 *  X-Macro Technique
 * ════════════════════════════════════════════════════════════════ */

/* X-macros let you define a list ONCE and expand it multiple ways.
 * This avoids the classic bug: adding an enum value but forgetting
 * to update the string table.  The list IS the single source of truth. */
#define COLOR_LIST \
    X(RED,   "Red",   0xFF0000) \
    X(GREEN, "Green", 0x00FF00) \
    X(BLUE,  "Blue",  0x0000FF) \
    X(WHITE, "White", 0xFFFFFF) \
    X(BLACK, "Black", 0x000000)

/* First expansion: generate enum values */
typedef enum {
#define X(name, str, hex) COLOR_##name,
    COLOR_LIST
#undef X
    COLOR_COUNT     /* sentinel — equals the number of entries */
} color_t;

/* Second expansion: generate string lookup table */
static const char *color_names[] = {
#define X(name, str, hex) str,
    COLOR_LIST
#undef X
};

/* Third expansion: generate hex value table */
static const unsigned int color_hex[] = {
#define X(name, str, hex) hex,
    COLOR_LIST
#undef X
};

/* Config struct for token-pasting demo */
static struct { int width; int height; int depth; } config = {1920, 1080, 24};
MAKE_GETTER(width)      /* expands to: int get_width(void) { ... }  */
MAKE_GETTER(height)
MAKE_GETTER(depth)

/* ════════════════════════════════════════════════════════════════
 *  Demo Functions
 * ════════════════════════════════════════════════════════════════ */

static void demo_object_macros(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Object-like Macros                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  PI         = %f\n", PI);
    printf("  MAX_SENSORS = %d\n", MAX_SENSORS);
    printf("  VERSION    = %s  (from common.h)\n\n", VERSION_STRING);

    printf("  What happens: The preprocessor does text replacement.\n");
    printf("  PI is NOT a variable — it's pasted into the source as\n");
    printf("  the literal 3.14159... everywhere it appears.\n\n");
}

static void demo_function_macros(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Function-like Macros & Hygiene          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  SQUARE(5)   = %d\n", SQUARE(5));
    printf("  SQUARE(1+2) = %d  (correct because of parentheses!)\n\n", SQUARE(1+2));

    /* Show the SWAP_INT do-while trick */
    int a = 10, b = 20;
    printf("  Before swap: a=%d, b=%d\n", a, b);
    SWAP_INT(a, b);         /* safe in any statement context */
    printf("  After swap:  a=%d, b=%d\n\n", a, b);

    /* Variadic macro demo */
    DEBUG_LOG("sensor count: %d", MAX_SENSORS);
    DEBUG_LOG("no args needed");     /* ## swallows the trailing comma */
    printf("\n");

    printf("  Why do-while(0)?\n");
    printf("    if (cond)          expands to      if (cond)\n");
    printf("      SWAP_INT(a,b);   ────────────>     do { ... } while(0);\n");
    printf("    else               ────────────>   else\n");
    printf("      ...              Works!  Without do-while it breaks.\n\n");
}

static void demo_stringify_concat(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Stringify (#) and Token Pasting (##)    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int sensor_temp = 72;
    int error_code  = -5;
    PRINT_VAR(sensor_temp);     /* prints: sensor_temp = 72    */
    PRINT_VAR(error_code);      /* prints: error_code = -5     */
    printf("\n");

    /* Token pasting: MAKE_GETTER generated real functions */
    printf("  get_width()  = %d  (generated by ## pasting)\n", get_width());
    printf("  get_height() = %d\n", get_height());
    printf("  get_depth()  = %d\n\n", get_depth());

    printf("  What happens: # turns the argument into a string literal.\n");
    printf("  PRINT_VAR(x) expands to printf(\"x = %%d\", x).\n");
    printf("  ## glues tokens: get_##width becomes get_width.\n\n");
}

static void demo_predefined(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Predefined Macros                       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  __FILE__    = %s\n", __FILE__);
    printf("  __LINE__    = %d\n", __LINE__);
    printf("  __DATE__    = %s\n", __DATE__);
    printf("  __TIME__    = %s\n", __TIME__);
    printf("  __func__    = %s  (C99, not technically preprocessor)\n", __func__);
#ifdef __STDC_VERSION__
    printf("  __STDC_VERSION__ = %ld", __STDC_VERSION__);
    if (__STDC_VERSION__ >= 201112L) printf("  (C11 or later)");
    else if (__STDC_VERSION__ >= 199901L) printf("  (C99)");
    printf("\n");
#endif
#ifdef __GNUC__
    printf("  __GNUC__    = %d  (GCC-compatible compiler)\n", __GNUC__);
#endif
    printf("\n");
}

static void demo_conditionals(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Conditional Compilation                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Platform detection — essential for portable code */
#if defined(__linux__)
    printf("  Platform: Linux\n");
#elif defined(_WIN32)
    printf("  Platform: Windows\n");
#elif defined(__APPLE__)
    printf("  Platform: macOS\n");
#else
    printf("  Platform: Unknown\n");
#endif

    /* Feature toggles — typically enabled via -DDEBUG on the command line */
#ifdef DEBUG
    printf("  DEBUG mode: ENABLED  (compiled with -DDEBUG)\n");
#else
    printf("  DEBUG mode: DISABLED (compile with -DDEBUG to enable)\n");
#endif

    /* Compile-time value tests */
#if MAX_SENSORS > 32
    printf("  MAX_SENSORS > 32: high-density sensor build\n");
#else
    printf("  MAX_SENSORS <= 32: standard sensor build\n");
#endif
    printf("\n");

    printf("  What happens: #ifdef/#if directives are evaluated at\n");
    printf("  compile time.  The 'dead' branches are removed entirely —\n");
    printf("  they generate zero machine code.  This is how you write\n");
    printf("  platform-specific code without runtime overhead.\n\n");
}

static void demo_x_macros(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: X-Macro Technique                       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  COLOR_LIST defined ONCE, expanded THREE ways:\n");
    printf("    1. enum color_t values\n");
    printf("    2. const char* name table\n");
    printf("    3. unsigned int hex table\n\n");

    printf("  ┌───────┬─────────────┬──────────┐\n");
    printf("  │ Enum  │ Name        │ Hex      │\n");
    printf("  ├───────┼─────────────┼──────────┤\n");
    for (int i = 0; i < COLOR_COUNT; i++) {
        printf("  │ %5d │ %-11s │ 0x%06X │\n", i, color_names[i], color_hex[i]);
    }
    printf("  └───────┴─────────────┴──────────┘\n\n");

    printf("  Why use X-macros?\n");
    printf("    • Single source of truth — add a color in ONE place\n");
    printf("    • Enum and string table stay in sync automatically\n");
    printf("    • Common in embedded: register maps, error codes, states\n\n");
}

static void demo_include_guards(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Include Guards vs #pragma once          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  Problem: if A.h includes B.h and C.h, and B.h also includes\n");
    printf("  C.h, then C.h gets included twice → duplicate definitions!\n\n");

    printf("  Solution 1: Include guards (standard C)\n");
    printf("    #ifndef MY_HEADER_H\n");
    printf("    #define MY_HEADER_H\n");
    printf("    ... contents ...\n");
    printf("    #endif /* MY_HEADER_H */\n\n");

    printf("  Solution 2: #pragma once (non-standard but universal)\n");
    printf("    #pragma once\n");
    printf("    ... contents ...\n\n");

    printf("  ┌─────────────────────┬─────────────────────────────┐\n");
    printf("  │ Include guards      │ #pragma once                │\n");
    printf("  ├─────────────────────┼─────────────────────────────┤\n");
    printf("  │ Standard C          │ Compiler extension          │\n");
    printf("  │ Works everywhere    │ Works on all major compilers│\n");
    printf("  │ Name collisions     │ No naming needed            │\n");
    printf("  │ possible            │                             │\n");
    printf("  │ Can guard partial   │ Guards whole file only      │\n");
    printf("  │ sections            │                             │\n");
    printf("  └─────────────────────┴─────────────────────────────┘\n\n");

    printf("  Our common.h uses include guards: #ifndef COMMON_H\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n=== C PREPROCESSOR DEMO v%s ===\n", VERSION_STRING);

    demo_object_macros();
    demo_function_macros();
    demo_stringify_concat();
    demo_predefined();
    demo_conditionals();
    demo_x_macros();
    demo_include_guards();

    DEMO_END();
    return 0;
}
