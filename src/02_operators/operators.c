/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 2 — C Operators                                        ║
 * ║  Modular-C-Demos                                                 ║
 * ║  Topics: arithmetic, relational, logical, bitwise, assignment,   ║
 * ║          miscellaneous, precedence, undefined behaviour           ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 *  Build:  make 02_operators
 *     or:  gcc -std=c11 -Wall -Wextra -o operators operators.c
 *
 *  Try these:
 *    1. Evaluate INT_MAX + 1  — signed overflow is undefined behaviour
 *    2. Try  b && (1/0)  vs  (1/0) && b  — short-circuit saves you
 *    3. Remove parens from a macro: #define SQ(x) x*x then call SQ(1+2)
 *    4. Right-shift a negative number — is it arithmetic or logical?
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Arithmetic Operators
 * ════════════════════════════════════════════════════════════════════ */
static void demo_arithmetic_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Arithmetic Operators                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int a = 17, b = 5;
    printf("  a = %d, b = %d\n\n", a, b);
    printf("  a + b  = %d   (addition)\n", a + b);
    printf("  a - b  = %d   (subtraction)\n", a - b);
    printf("  a * b  = %d   (multiplication)\n", a * b);

    /* Integer division truncates toward zero (C99+).
     * 17 / 5 = 3, not 3.4 — the fractional part is discarded. */
    printf("  a / b  = %d   (integer division — truncates toward zero)\n", a / b);

    /* Modulus gives the remainder.  Sign follows the dividend in C99+. */
    printf("  a %% b = %d   (modulus — remainder of division)\n\n", a % b);

    /* Pre-increment vs post-increment: both add 1, but differ in
     * WHEN the value is returned.  Mixing them in one expression
     * can create sequence-point violations (undefined behaviour). */
    int x = 10;
    printf("  Pre-increment:  ++x = %d  (increment, then return)\n", ++x);
    printf("  Post-increment: x++ = %d  (return, then increment)\n", x++);
    printf("  After post-inc: x   = %d\n", x);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Relational Operators
 * ════════════════════════════════════════════════════════════════════ */
static void demo_relational_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Relational Operators                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int a = 10, b = 20;
    printf("  a = %d, b = %d\n\n", a, b);

    /* All relational operators return 1 (true) or 0 (false). */
    printf("  a == b: %d   (equality — note: = is assignment!)\n", a == b);
    printf("  a != b: %d   (inequality)\n", a != b);
    printf("  a >  b: %d   (greater than)\n", a > b);
    printf("  a <  b: %d   (less than)\n", a < b);
    printf("  a >= b: %d   (greater or equal)\n", a >= b);
    printf("  a <= b: %d   (less or equal)\n\n", a <= b);

    /* Common bug: writing  if (x = 5)  instead of  if (x == 5).
     * The first assigns 5 to x and is always true.  Some compilers
     * warn; use -Wall to catch this. */
    printf("  Gotcha: 'if (x = 5)' assigns, 'if (x == 5)' compares!\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Logical Operators & Short-Circuit Evaluation
 * ════════════════════════════════════════════════════════════════════ */
static void demo_logical_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Logical Operators & Short-Circuit           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int a = 1, b = 0;
    printf("  a = %d (truthy), b = %d (falsy)\n\n", a, b);

    printf("  a && b (AND): %d   (both must be true)\n", a && b);
    printf("  a || b (OR):  %d   (at least one true)\n", a || b);
    printf("  !a     (NOT): %d   (inverts truthiness)\n", !a);
    printf("  !b     (NOT): %d\n\n", !b);

    /* Short-circuit evaluation: && stops at first false, || stops at
     * first true.  The right operand is NEVER evaluated if the result
     * is already determined.  This is GUARANTEED by the C standard —
     * it introduces a sequence point. */
    printf("  Short-circuit demo:\n");
    printf("    b && (1/0)  → b is 0, so RHS is never evaluated\n");
    printf("    a || (1/0)  → a is 1, so RHS is never evaluated\n\n");

    /* This pattern is idiomatic for null-pointer guards: */
    int *ptr = NULL;
    printf("  Guard: ptr && *ptr  → safe, won't dereference NULL\n");
    printf("  Result: %d\n", ptr && *ptr);

    printf("\n  What happens: Short-circuit is not just an optimisation —\n");
    printf("  it creates a sequence point.  Side effects on the left are\n");
    printf("  complete before the right side is evaluated.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Bitwise Operators
 * ════════════════════════════════════════════════════════════════════ */
static void demo_bitwise_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Bitwise Operators                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    unsigned int a = 0xCC, b = 0xAA;  /* 11001100, 10101010 */
    printf("  a = 0x%02X (11001100)\n", a);
    printf("  b = 0x%02X (10101010)\n\n", b);

    printf("  a & b  (AND):  0x%02X  (bits set in BOTH)\n", a & b);
    printf("  a | b  (OR):   0x%02X  (bits set in EITHER)\n", a | b);
    printf("  a ^ b  (XOR):  0x%02X  (bits that DIFFER)\n", a ^ b);
    printf("  ~a     (NOT):  0x%08X  (all bits flipped — note: 32-bit)\n", ~a);
    printf("  a << 2 (LSH):  0x%03X  (multiply by 4)\n", a << 2);
    printf("  a >> 2 (RSH):  0x%02X  (divide by 4)\n\n", a >> 2);

    /* Using macros from common.h for embedded-style bit manipulation */
    u32 flags = 0;
    SET_BIT(flags, 3);                   /* Set bit 3 → flags = 0x08 */
    SET_BIT(flags, 7);                   /* Set bit 7 → flags = 0x88 */
    printf("  BIT macro demo:  SET_BIT(0, 3) | SET_BIT(0, 7) = 0x%02X\n", flags);
    printf("  CHECK_BIT(flags, 3) = %d\n", CHECK_BIT(flags, 3));
    TOGGLE_BIT(flags, 3);               /* Toggle bit 3 off */
    printf("  After TOGGLE_BIT(3): 0x%02X\n", flags);

    /* Warning: right-shifting a signed negative number is
     * implementation-defined — it may be arithmetic (sign-extending)
     * or logical (zero-filling).  Use unsigned types for bit ops! */
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Assignment Operators
 * ════════════════════════════════════════════════════════════════════ */
static void demo_assignment_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Assignment Operators                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int x = 10;
    printf("  x = %d\n", x);
    x += 5;  printf("  x += 5:   %d\n", x);
    x -= 3;  printf("  x -= 3:   %d\n", x);
    x *= 2;  printf("  x *= 2:   %d\n", x);
    x /= 4;  printf("  x /= 4:   %d\n", x);
    x %= 3;  printf("  x %%= 3:  %d\n\n", x);

    /* Compound assignment with bitwise ops — common in register config */
    x = 0x0F;
    x &= 0x0A;  printf("  x &= 0x0A: 0x%02X  (mask off bits)\n", x);
    x |= 0x05;  printf("  x |= 0x05: 0x%02X  (set bits)\n", x);
    x ^= 0x0F;  printf("  x ^= 0x0F: 0x%02X  (toggle bits)\n", x);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Miscellaneous Operators
 * ════════════════════════════════════════════════════════════════════ */
static void demo_misc_operators(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Miscellaneous Operators                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Ternary: the only C operator that takes 3 operands */
    int a = 5, b = 10;
    printf("  Ternary:  (a > b) ? a : b = %d\n", (a > b) ? a : b);

    /* sizeof: evaluated at COMPILE time (except for VLAs).
     * Returns a size_t — always print with %zu. */
    printf("  sizeof(int) = %zu  (compile-time, not a function call)\n", sizeof(int));

    /* Comma operator: evaluates left-to-right, returns rightmost value.
     * Rarely used outside for-loop headers. */
    int result = (a = 1, b = 2, a + b);
    printf("  Comma:    (a=1, b=2, a+b) = %d\n\n", result);

    /* Pointer & address operators */
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;
    printf("  *p (dereference):  %d  (follow the pointer)\n", *p);
    printf("  &a (address-of):   %p\n", (void*)&a);
    printf("  arr[2] (subscript): %d  (equivalent to *(arr+2))\n\n", arr[2]);

    /* Member access operators */
    struct demo_s { int x; };
    struct demo_s s = {42};
    struct demo_s *sp = &s;
    printf("  s.x  (dot):       %d  (direct member access)\n", s.x);
    printf("  sp->x (arrow):    %d  (shorthand for (*sp).x)\n", sp->x);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Operator Precedence & Undefined Behaviour
 * ════════════════════════════════════════════════════════════════════ */
static void demo_precedence_and_ub(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Precedence & Undefined Behaviour            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int a = 2, b = 3, c = 4;
    printf("  a + b * c     = %d  (mul binds tighter than add)\n", a + b * c);
    printf("  (a + b) * c   = %d  (parens override precedence)\n", (a + b) * c);
    printf("  a || b && c   = %d  (&& binds tighter than ||)\n\n", a || (b && c));

    /* Precedence table (highest to lowest):
     *   ()  []  ->  .              — postfix
     *   !  ~  ++  --  (type)  *  &  sizeof  — unary
     *   *  /  %%                   — multiplicative
     *   +  -                      — additive
     *   <<  >>                    — shift
     *   <  <=  >  >=             — relational
     *   ==  !=                    — equality
     *   &  ^  |                   — bitwise (in that order!)
     *   &&  ||                    — logical (in that order!)
     *   ?:                        — ternary
     *   =  +=  -=  ...           — assignment
     *   ,                         — comma (lowest)
     */

    printf("  Why parenthesise macro args:\n");
    printf("    #define SQ(x) x*x    → SQ(1+2) = 1+2*1+2 = 5  (WRONG)\n");
    printf("    #define SQ(x) ((x)*(x)) → SQ(1+2) = ((3)*(3)) = 9  (RIGHT)\n\n");

    printf("  Undefined Behaviour examples (NEVER do these):\n");
    printf("    - Signed integer overflow:  INT_MAX + 1\n");
    printf("    - Unsequenced modifications: i = i++ + ++i\n");
    printf("    - Shift by negative or >= bit-width: 1 << 32\n");
    printf("    - Division by zero: x / 0\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 2 — C Operators  v%s                           ║\n",
           VERSION_STRING);
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_arithmetic_operators();
    demo_relational_operators();
    demo_logical_operators();
    demo_bitwise_operators();
    demo_assignment_operators();
    demo_misc_operators();
    demo_precedence_and_ub();

    DEMO_END();
    return 0;
}
