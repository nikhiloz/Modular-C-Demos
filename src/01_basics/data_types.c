/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 1 — C Data Types                                       ║
 * ║  Modular-C-Demos                                                 ║
 * ║  Topics: integers, floats, fixed-width, qualifiers, storage,     ║
 * ║          derived types, booleans, void, type conversions          ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 *  Build:  make 01_basics
 *     or:  gcc -std=c11 -Wall -Wextra -o data_types data_types.c
 *
 *  Try these:
 *    1. Change INT_MIN to INT_MAX+1 and observe signed overflow (UB!)
 *    2. Print sizeof(long) on a 32-bit vs 64-bit system — it differs
 *    3. Remove 'volatile' from hw_reg and compile with -O2 — the
 *       compiler may optimise the read away entirely
 *    4. Cast a negative int to unsigned and inspect the result
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Integer Data Types
 * ════════════════════════════════════════════════════════════════════ */
static void demo_integer_types(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Integer Data Types                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* char: guaranteed at least 8 bits.  Whether plain 'char' is signed
     * or unsigned is implementation-defined — never assume! */
    char c = 65;                         /* ASCII 'A' */
    signed char sc = -128;               /* Minimum for 8-bit signed */
    unsigned char uc = 255;              /* Maximum for 8-bit unsigned */
    printf("  char:           '%c'  (size: %zu byte)\n", c, sizeof(c));
    printf("  signed char:    %d    (range: -128 to 127)\n", sc);
    printf("  unsigned char:  %u    (range: 0 to 255)\n\n", uc);

    /* short: at least 16 bits.  Rarely used alone — mostly seen in
     * network protocols and file formats with fixed layouts. */
    short s = -32768;
    unsigned short us = 65535;
    printf("  short:          %hd   (size: %zu bytes)\n", s, sizeof(s));
    printf("  unsigned short: %hu\n\n", us);

    /* int: the "natural" word size — at least 16 bits, almost always 32.
     * This is the default type for integer arithmetic in C. */
    int i = -2147483647;
    unsigned int ui = 4294967295U;       /* The 'U' suffix avoids warnings */
    printf("  int:            %d  (size: %zu bytes)\n", i, sizeof(i));
    printf("  unsigned int:   %u\n\n", ui);

    /* long: at least 32 bits.  On LP64 (Linux 64-bit) it's 8 bytes;
     * on LLP64 (Windows 64-bit) it's still 4 bytes — beware! */
    long l = -2147483648L;
    unsigned long ul = 4294967295UL;
    printf("  long:           %ld  (size: %zu bytes)\n", l, sizeof(l));
    printf("  unsigned long:  %lu\n\n", ul);

    /* long long: guaranteed at least 64 bits (C99+). */
    long long ll = -9223372036854775807LL;
    printf("  long long:      %lld  (size: %zu bytes)\n", ll, sizeof(ll));
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Floating Point Types
 * ════════════════════════════════════════════════════════════════════ */
static void demo_floating_types(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Floating Point Types                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* IEEE 754 representations — the 'f' suffix forces float literal,
     * otherwise C defaults to double for floating constants. */
    float f = 3.14159265f;               /* ~7 decimal digits precision */
    double d = 3.141592653589793;        /* ~15 decimal digits precision */
    long double ld = 3.14159265358979323846L;  /* ≥15, often 18-19 digits */

    printf("  float:       %.7f   (size: %zu, precision: %d digits)\n",
           f, sizeof(f), FLT_DIG);
    printf("  double:      %.15lf (size: %zu, precision: %d digits)\n",
           d, sizeof(d), DBL_DIG);
    printf("  long double: %.18Lf (size: %zu)\n\n", ld, sizeof(ld));

    /* What happens: FLT_DIG (typically 6) is the number of decimal
     * digits that survive a round-trip float→decimal→float.  This is
     * why 0.1f + 0.2f != 0.3f — binary can't represent 0.1 exactly. */
    printf("  Limits: FLT_MAX = %e\n", FLT_MAX);
    printf("          DBL_MAX = %e\n", DBL_MAX);
    printf("  Gotcha: 0.1 + 0.2 = %.17f (not 0.3!)\n", 0.1 + 0.2);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Fixed-Width Integer Types (stdint.h)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_fixed_width_types(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Fixed-Width Types (stdint.h / common.h)     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Fixed-width types solve the "how big is long?" portability problem.
     * Use these whenever you need an exact bit width — embedded code,
     * protocols, serialisation. */
    printf("  int8_t  (i8):   min = %d,   max = %d    (%zu byte)\n",
           INT8_MIN, INT8_MAX, sizeof(i8));
    printf("  int16_t (i16):  min = %d, max = %d  (%zu bytes)\n",
           INT16_MIN, INT16_MAX, sizeof(i16));
    printf("  int32_t (i32):  min = %d, max = %d  (%zu bytes)\n",
           INT32_MIN, INT32_MAX, sizeof(i32));
    printf("  uint8_t (u8):   max = %u                (%zu byte)\n",
           UINT8_MAX, sizeof(u8));
    printf("  uint16_t(u16):  max = %u              (%zu bytes)\n",
           UINT16_MAX, sizeof(u16));
    printf("  uint32_t(u32):  max = %u         (%zu bytes)\n\n",
           UINT32_MAX, sizeof(u32));

    /* size_t: unsigned type returned by sizeof — always use %zu */
    printf("  size_t:     %zu bytes (holds any object size)\n", sizeof(size_t));
    printf("  ptrdiff_t:  %zu bytes (holds pointer differences)\n", sizeof(ptrdiff_t));
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Type Qualifiers
 * ════════════════════════════════════════════════════════════════════ */
static void demo_type_qualifiers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Type Qualifiers (const / volatile / restrict)║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* const: tells the compiler this value must not be modified.
     * Attempting to write through a const pointer is UB. */
    const int readonly = 100;
    printf("  const int = %d  (compiler enforces read-only)\n", readonly);

    /* volatile: tells the compiler "this value can change behind your back"
     * — do NOT optimise reads/writes away.  Used for:
     *   - Memory-mapped hardware registers
     *   - Variables modified by signal handlers
     *   - Variables shared with ISRs (interrupt service routines) */
    volatile int hw_reg = 42;
    printf("  volatile int = %d  (every read hits memory, never cached)\n", hw_reg);

    /* restrict (C99): a promise to the compiler that this pointer is the
     * ONLY way to access the pointed-to object.  Enables aggressive
     * optimisations (like vectorisation).  Used in memcpy, etc. */
    int arr[5] = {1, 2, 3, 4, 5};
    int * restrict p = arr;              /* "I won't alias this pointer" */
    printf("  restrict ptr = %d  (enables compiler optimisations)\n\n", *p);

    printf("  What happens: Without 'volatile', a variable read in a loop\n");
    printf("  might be hoisted out by -O2.  The compiler assumes memory\n");
    printf("  doesn't change unless the current code changes it.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Storage Classes
 * ════════════════════════════════════════════════════════════════════ */
static void demo_storage_classes(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Storage Classes                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* auto: default for local variables — lives on the stack, destroyed
     * when the enclosing scope exits.  The keyword is almost never used. */
    auto int a = 1;
    printf("  auto:     %d  (stack-allocated, default for locals)\n", a);

    /* static inside a function: persists across calls — stored in .data
     * or .bss segment, NOT on the stack.  Initialised once at startup. */
    static int call_count = 0;
    call_count++;
    printf("  static:   %d  (persists across calls, in .data segment)\n", call_count);

    /* register: a HINT to the compiler to keep this in a CPU register.
     * Modern compilers ignore it — they do register allocation better
     * than humans.  You cannot take the address of a register variable. */
    register int r = 42;
    printf("  register: %d  (hint only, compiler may ignore)\n", r);
    printf("            Note: &r is illegal — can't take address\n");

    /* extern: declares a variable defined elsewhere (another .c file).
     * Not demonstrated here — see multi-file linking examples. */
    printf("  extern:   (declares variable from another translation unit)\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Derived Types
 * ════════════════════════════════════════════════════════════════════ */
static void demo_derived_types(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Derived Types (arrays, pointers, enums)     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Array: contiguous block, sizeof gives TOTAL bytes */
    int arr[5] = {1, 2, 3, 4, 5};
    printf("  Array[5]: sizeof = %zu  (%zu elements × %zu bytes each)\n",
           sizeof(arr), ARRAY_SIZE(arr), sizeof(arr[0]));

    /* Pointer: holds a memory address — sizeof is always the machine
     * word size (8 bytes on 64-bit, 4 on 32-bit) */
    int *ptr = arr;
    printf("  Pointer:  sizeof = %zu  (address: %p → value: %d)\n",
           sizeof(ptr), (void*)ptr, *ptr);

    /* Enum: integer constants with names — sizeof is typically 4 (int) */
    enum Color { RED = 1, GREEN = 2, BLUE = 4 };
    enum Color c = GREEN;
    printf("  Enum:     sizeof = %zu  (GREEN = %d)\n", sizeof(c), c);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Boolean and Void Types
 * ════════════════════════════════════════════════════════════════════ */
static void demo_bool_and_void(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Boolean and Void Types                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* bool (C99 via <stdbool.h>): stored as 1 byte, but only 0 or 1.
     * In C, ANY non-zero value is truthy. */
    bool t = true, f = false;
    printf("  bool: true = %d, false = %d  (sizeof = %zu)\n", t, f, sizeof(t));
    printf("  C truthiness: 42 is %s, 0 is %s\n\n",
           42 ? "true" : "false", 0 ? "true" : "false");

    /* void*: generic pointer — can hold any data pointer.
     * Must cast back to the correct type before dereferencing. */
    int x = 42;
    void *gp = &x;                      /* No cast needed to void* */
    printf("  void* generic pointer → cast back → value: %d\n", *(int*)gp);
    printf("  void* is how malloc returns memory (type-agnostic)\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Type Conversion Pitfalls
 * ════════════════════════════════════════════════════════════════════ */
static void demo_type_conversions(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Type Conversion Pitfalls                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Implicit widening: safe — smaller type promoted to larger */
    int small = 42;
    double wide = small;                 /* int → double, no data loss */
    printf("  Widening:  int %d → double %.1f  (safe)\n", small, wide);

    /* Implicit narrowing: DANGEROUS — data may be lost silently */
    double big = 3.999;
    int truncated = (int)big;            /* Fractional part discarded */
    printf("  Narrowing: double %.3f → int %d  (truncated!)\n", big, truncated);

    /* Signed ↔ Unsigned: the bit pattern is reinterpreted, not converted */
    int neg = -1;
    unsigned int as_unsigned = (unsigned int)neg;
    printf("  Signed→Unsigned: -1 becomes %u  (all bits set)\n", as_unsigned);

    /* Integer promotion: types smaller than int are promoted to int
     * before any arithmetic — this surprises many beginners */
    u8 a = 200, b = 100;
    /* a + b is computed as int (300), then assigned.  If target were u8,
     * the result would wrap to 44 (300 - 256). */
    int sum = a + b;                     /* Promoted to int first */
    u8 wrapped = (u8)(a + b);           /* Wraps: 300 mod 256 = 44 */
    printf("  Promotion: u8(200) + u8(100) = int(%d), u8(%u)\n\n", sum, wrapped);

    printf("  What happens: The C standard says signed overflow is\n");
    printf("  undefined behaviour.  The compiler can assume it never\n");
    printf("  happens and optimise accordingly — never rely on it!\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 1 — C Data Types  v%s                          ║\n",
           VERSION_STRING);
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_integer_types();
    demo_floating_types();
    demo_fixed_width_types();
    demo_type_qualifiers();
    demo_storage_classes();
    demo_derived_types();
    demo_bool_and_void();
    demo_type_conversions();

    DEMO_END();
    return 0;
}
