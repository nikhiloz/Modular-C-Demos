/*
 * Chapter 13 — Advanced C Features (C99/C11)
 *
 * C is not a frozen language.  C99 and C11 added features that make
 * code safer, more expressive, and closer to what modern compilers
 * can optimise.  This chapter covers the most useful additions.
 *
 * Topics covered:
 *   1. Compound literals — anonymous arrays and structs
 *   2. Designated initializers — name what you're initializing
 *   3. _Generic (C11) — type-generic expressions
 *   4. _Static_assert (C11) — compile-time assertions
 *   5. typeof (GCC extension) — deduce types in macros
 *   6. Flexible array members — variable-length struct tails
 *   7. Anonymous structs/unions (C11)
 *
 * Build: make 13_advanced   (uses -std=c11)
 * Run:   ./bin/13_advanced
 *
 * Try these:
 *   gcc -std=c99 -fsyntax-only src/13_advanced/advanced.c  # C99 vs C11 errors
 *   gcc -std=c11 -E src/13_advanced/advanced.c | grep _Static_assert
 *   gcc -std=c11 -Wno-unused src/13_advanced/advanced.c    # experiment with warnings
 */

#include "../../include/common.h"
#include <math.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Compound Literals
 * ════════════════════════════════════════════════════════════════ */

/* A compound literal creates an unnamed object with a given type.
 * Syntax: (type){ initializer-list }
 * Lifetime: if at block scope, same as an automatic variable.
 *           if at file scope, static storage duration.          */

typedef struct { float x, y; } point_t;
typedef struct { u8 r, g, b; } color_rgb_t;

/* Functions that take struct arguments by value — compound literals
 * let us call them without declaring a named variable first. */
static float point_distance(point_t a, point_t b)
{
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

static void print_color(const char *name, color_rgb_t c)
{
    printf("  %-10s = rgb(%3d, %3d, %3d)\n", name, c.r, c.g, c.b);
}

static void demo_compound_literals(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Compound Literals (C99)                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Pass anonymous struct directly — no temp variable needed */
    float d = point_distance((point_t){0, 0}, (point_t){3, 4});
    printf("  Distance (0,0)→(3,4) = %.1f\n\n", d);

    /* Anonymous array — the (int[]){...} creates a real array in memory */
    int *primes = (int[]){2, 3, 5, 7, 11, 13};
    printf("  Anonymous int array: ");
    for (int i = 0; i < 6; i++) printf("%d ", primes[i]);
    printf("\n\n");

    /* Compound literal structs as function arguments */
    print_color("Red",   (color_rgb_t){255,   0,   0});
    print_color("Green", (color_rgb_t){  0, 255,   0});
    print_color("Blue",  (color_rgb_t){  0,   0, 255});
    printf("\n");

    printf("  What happens: (type){values} creates a temporary object.\n");
    printf("  It's a real lvalue — you can take its address.  Great for\n");
    printf("  passing one-off structs or arrays to functions.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Designated Initializers
 * ════════════════════════════════════════════════════════════════ */

typedef struct {
    char name[32];
    int  age;
    float gpa;
    bool enrolled;
} student_t;

static void demo_designated_init(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Designated Initializers (C99)           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Array: initialize specific indices — rest become 0 */
    int sparse[10] = {
        [0] = 100,
        [3] = 300,
        [7] = 700,
        [9] = 900
    };
    printf("  Sparse array [10]: ");
    for (int i = 0; i < 10; i++) printf("%3d ", sparse[i]);
    printf("\n  (unspecified elements are zero-initialised)\n\n");

    /* Struct: name the fields — order doesn't matter, unmentioned = 0 */
    student_t alice = {
        .name     = "Alice",
        .gpa      = 3.85f,
        .enrolled = true
        /* .age not specified → defaults to 0 */
    };
    printf("  Student: %s, age=%d, gpa=%.2f, enrolled=%s\n",
           alice.name, alice.age, alice.gpa,
           alice.enrolled ? "yes" : "no");
    printf("  (.age was omitted → zero-initialised to %d)\n\n", alice.age);

    /* Mix of positional and designated (legal but avoid for clarity) */
    int mixed[5] = {1, 2, [4] = 50};
    printf("  Mixed init [5]: ");
    for (int i = 0; i < 5; i++) printf("%d ", mixed[i]);
    printf("\n\n");

    printf("  What happens: designated initializers let you specify\n");
    printf("  which element/field you're setting by NAME or INDEX.\n");
    printf("  Much more readable than positional init for large structs.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: _Generic (C11 Type-Generic Expressions)
 * ════════════════════════════════════════════════════════════════ */

/* _Generic selects an expression based on the TYPE of its first argument.
 * It's like a compile-time switch on types.  This is how <tgmath.h>
 * implements sin/cos/sqrt that work for float, double, and long double. */

#define type_name(x) _Generic((x),          \
    int:          "int",                     \
    float:        "float",                   \
    double:       "double",                  \
    char:         "char",                    \
    char*:        "char*",                   \
    const char*:  "const char*",             \
    int*:         "int*",                    \
    default:      "unknown"                  \
)

/* Type-safe abs() — selects the right function at compile time */
#define generic_abs(x) _Generic((x),        \
    int:    abs,                             \
    float:  fabsf,                           \
    double: fabs                             \
)(x)

static void demo_generic(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: _Generic (C11 Type Selection)           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int    i = 42;
    float  f = 3.14f;
    double d = 2.718;
    char   c = 'A';
    char  *s = "hello";

    printf("  type_name(42)      = %s\n", type_name(i));
    printf("  type_name(3.14f)   = %s\n", type_name(f));
    printf("  type_name(2.718)   = %s\n", type_name(d));
    printf("  type_name('A')     = %s\n", type_name(c));
    printf("  type_name(\"hello\") = %s\n\n", type_name(s));

    printf("  generic_abs(-5)    = %d  (dispatches to abs)\n", generic_abs(-5));
    printf("  generic_abs(-3.1f) = %.1f  (dispatches to fabsf)\n",
           generic_abs(-3.1f));
    printf("  generic_abs(-2.7)  = %.1f  (dispatches to fabs)\n\n",
           generic_abs(-2.7));

    printf("  What happens: _Generic is resolved at COMPILE time.\n");
    printf("  The compiler picks the matching branch based on the\n");
    printf("  expression's type.  No runtime overhead — it's like\n");
    printf("  C++'s function overloading but done via the preprocessor.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: _Static_assert (C11 Compile-Time Checks)
 * ════════════════════════════════════════════════════════════════ */

/* These assertions are checked at COMPILE time, not runtime.
 * If the condition is false, compilation fails with the message.
 * Use them to catch platform assumptions early. */

_Static_assert(sizeof(int) >= 4,
    "This code requires int to be at least 32 bits");

_Static_assert(sizeof(void*) == 8 || sizeof(void*) == 4,
    "Expected 32-bit or 64-bit pointer size");

_Static_assert(sizeof(u8)  == 1, "u8 must be 1 byte");
_Static_assert(sizeof(u16) == 2, "u16 must be 2 bytes");
_Static_assert(sizeof(u32) == 4, "u32 must be 4 bytes");

/* Struct layout assertion — catch accidental padding changes */
typedef struct { u8 type; u32 value; } __attribute__((packed)) packed_msg_t;
_Static_assert(sizeof(packed_msg_t) == 5,
    "packed_msg_t should be exactly 5 bytes");

static void demo_static_assert(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: _Static_assert (C11 Compile-Time)       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  All static assertions above passed (or this wouldn't compile):\n");
    printf("    sizeof(int)    = %zu bytes  (≥ 4 required)\n", sizeof(int));
    printf("    sizeof(void*)  = %zu bytes  (4 or 8)\n", sizeof(void*));
    printf("    sizeof(u8)     = %zu byte\n", sizeof(u8));
    printf("    sizeof(u16)    = %zu bytes\n", sizeof(u16));
    printf("    sizeof(u32)    = %zu bytes\n", sizeof(u32));
    printf("    sizeof(packed_msg_t) = %zu bytes  (packed)\n\n", sizeof(packed_msg_t));

    printf("  What happens: _Static_assert is checked at compile time.\n");
    printf("  If it fails, the compiler prints your error message and stops.\n");
    printf("  Use it to verify platform assumptions (sizes, alignment)\n");
    printf("  instead of hoping they hold true at runtime.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: typeof (GCC Extension)
 * ════════════════════════════════════════════════════════════════ */

/* __typeof__ (GCC/Clang) deduces the type of an expression.
 * C23 standardises plain 'typeof'.  Until then, __typeof__ works
 * even with -std=c11.  Essential for type-safe macros. */

/* Type-safe swap — works for any type, evaluates args only once.
 * __typeof__ is the GCC/Clang spelling that works with -std=c11.
 * (Plain typeof becomes standard in C23.) */
#define TYPED_SWAP(a, b) do {           \
    __typeof__(a) _tmp = (a);           \
    (a) = (b);                          \
    (b) = _tmp;                         \
} while (0)

/* Type-safe max — avoids double-evaluation of a/b.
 * ({ ... }) is a GCC statement expression (non-standard). */
#define TYPED_MAX(a, b) __extension__({ \
    __typeof__(a) _a = (a);             \
    __typeof__(b) _b = (b);             \
    _a > _b ? _a : _b;                 \
})

static void demo_typeof(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: __typeof__ (GCC/Clang Extension)         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int x = 10, y = 20;
    printf("  Before TYPED_SWAP: x=%d, y=%d\n", x, y);
    TYPED_SWAP(x, y);
    printf("  After TYPED_SWAP:  x=%d, y=%d\n\n", x, y);

    float a = 3.14f, b = 2.71f;
    printf("  Before TYPED_SWAP: a=%.2f, b=%.2f\n", a, b);
    TYPED_SWAP(a, b);
    printf("  After TYPED_SWAP:  a=%.2f, b=%.2f\n\n", a, b);

    printf("  TYPED_MAX(100, 200)     = %d\n", TYPED_MAX(100, 200));
    printf("  TYPED_MAX(3.14f, 2.71f) = %.2f\n\n", TYPED_MAX(3.14f, 2.71f));

    printf("  What happens: __typeof__(expr) yields the type of expr.\n");
    printf("  TYPED_SWAP works for int, float, double, pointers — any type.\n");
    printf("  The compiler deduces _tmp's type from the argument.\n");
    printf("  C23 standardises plain 'typeof'; until then use __typeof__.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Flexible Array Members (C99)
 * ════════════════════════════════════════════════════════════════ */

/* A flexible array member is an unsized array at the END of a struct.
 * The struct is allocated with extra space to hold the data.
 * The struct itself only stores the metadata (length, etc.).
 * This is the standard pattern for variable-length messages. */

typedef struct {
    u32 length;         /* how many elements follow        */
    u32 checksum;       /* metadata before the payload     */
    u8  data[];         /* flexible array — size determined at malloc time */
} packet_t;

static void demo_flexible_array(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Flexible Array Members (C99)            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* sizeof(packet_t) does NOT include data[] — just the fixed fields */
    printf("  sizeof(packet_t) = %zu  (just length + checksum, no data)\n\n",
           sizeof(packet_t));

    /* Allocate a packet with room for 10 bytes of payload */
    u32 payload_len = 10;
    packet_t *pkt = malloc(sizeof(packet_t) + payload_len * sizeof(u8));
    if (!pkt) { printf("  malloc failed\n"); return; }

    pkt->length   = payload_len;
    pkt->checksum = 0;
    for (u32 i = 0; i < payload_len; i++) {
        pkt->data[i] = (u8)(i * 11);       /* fill with sample data */
        pkt->checksum += pkt->data[i];      /* simple additive checksum */
    }

    printf("  Packet: length=%u, checksum=%u\n", pkt->length, pkt->checksum);
    printf("  Data:   ");
    for (u32 i = 0; i < pkt->length; i++) printf("%02X ", pkt->data[i]);
    printf("\n\n");

    free(pkt);

    printf("  What happens: data[] has zero size in the struct layout.\n");
    printf("  You allocate sizeof(struct) + N * sizeof(element) and the\n");
    printf("  extra memory IS the array.  Common in network packets,\n");
    printf("  file formats, and kernel data structures.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Anonymous Structs and Unions (C11)
 * ════════════════════════════════════════════════════════════════ */

/* Anonymous structs/unions let you access nested members directly
 * without naming the intermediate struct/union field.
 * Very common in hardware register definitions and vector types. */

typedef struct {
    union {
        struct { float x, y, z; };      /* anonymous struct inside union */
        float components[3];            /* access as array too           */
    };                                   /* anonymous union — no field name */
    float w;                             /* separate field */
} vec4_t;

typedef struct {
    u32 id;
    union {
        struct { u16 major, minor; };   /* version as two halves */
        u32 version;                     /* or as a single u32   */
    };
} device_t;

static void demo_anonymous(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Anonymous Structs & Unions (C11)        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Access .x/.y/.z directly — no intermediate field name */
    vec4_t v = { .x = 1.0f, .y = 2.0f, .z = 3.0f, .w = 1.0f };
    printf("  vec4: (%.1f, %.1f, %.1f, %.1f)\n", v.x, v.y, v.z, v.w);

    /* Same memory accessed as array */
    printf("  As array: ");
    for (int i = 0; i < 3; i++) printf("%.1f ", v.components[i]);
    printf("\n\n");

    /* Device: version accessible as one u32 or two u16 halves */
    device_t dev = { .id = 42, .major = 2, .minor = 5 };
    printf("  Device id=%u, version=0x%08X (major=%u, minor=%u)\n",
           dev.id, dev.version, dev.major, dev.minor);
    printf("\n");

    printf("  What happens: anonymous struct/union members are\n");
    printf("  promoted to the enclosing struct's namespace.\n");
    printf("  v.x and v.components[0] alias the same memory.\n");
    printf("  Eliminates ugly v.pos.x patterns in maths code.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n=== ADVANCED C FEATURES DEMO v%s ===\n", VERSION_STRING);

    demo_compound_literals();
    demo_designated_init();
    demo_generic();
    demo_static_assert();
    demo_typeof();
    demo_flexible_array();
    demo_anonymous();

    DEMO_END();
    return 0;
}
