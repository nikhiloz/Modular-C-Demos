/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 22 — Optimisation Passes                               ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: Constant folding, DCE, strength reduction, inlining    ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Why Optimise?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_why_optimise(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Why Optimise?                              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The compiler transforms IR to produce faster, smaller, or\n");
    printf("more power-efficient code — without changing behaviour.\n\n");

    printf("GCC optimisation levels:\n");
    printf("  ┌────────┬──────────────────────────────────────────────┐\n");
    printf("  │ Flag   │ Meaning                                     │\n");
    printf("  ├────────┼──────────────────────────────────────────────┤\n");
    printf("  │ -O0    │ No optimisation (default) — best for debug  │\n");
    printf("  │ -O1    │ Basic optimisations, fast compile           │\n");
    printf("  │ -O2    │ Most optimisations, good balance            │\n");
    printf("  │ -O3    │ Aggressive: vectorise, unroll, inline more  │\n");
    printf("  │ -Os    │ Optimise for code size                      │\n");
    printf("  │ -Og    │ Optimise for debugging experience           │\n");
    printf("  │ -Ofast │ -O3 + fast-math (may break IEEE float)      │\n");
    printf("  └────────┴──────────────────────────────────────────────┘\n\n");

    printf("Key principle: optimisations must preserve observable behaviour.\n");
    printf("(The 'as-if' rule — the result must be *as if* unoptimised.)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Constant Folding
 * ════════════════════════════════════════════════════════════════════ */
static void demo_constant_folding(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Constant Folding                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Constant folding evaluates expressions whose operands are\n");
    printf("known at compile time, replacing them with the result.\n\n");

    printf("  Before:  int x = 3 * 4 + 1;\n");
    printf("  After:   int x = 13;          ← computed at compile time\n\n");

    printf("  Before:  int y = sizeof(int) * 8;\n");
    printf("  After:   int y = 32;\n\n");

    /* Demonstrate: the compiler will fold this */
    int a = 3 * 4 + 1;   /* compiler folds to 13 */
    int b = 100 / 5 - 3;  /* compiler folds to 17 */
    printf("  Demo: a = 3*4+1 = %d, b = 100/5-3 = %d\n", a, b);
    printf("  (At -O2, no multiply/divide instructions are generated.)\n\n");

    printf("  Constant propagation extends this:\n");
    printf("    int width = 10;\n");
    printf("    int height = 20;\n");
    printf("    int area = width * height;  →  int area = 200;\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Compare assembly:                                      ║\n");
    printf("║  gcc -O0 -S -o opt_O0.s optimisation.c                  ║\n");
    printf("║  gcc -O2 -S -o opt_O2.s optimisation.c                  ║\n");
    printf("║  diff opt_O0.s opt_O2.s                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Dead Code Elimination (DCE)
 * ════════════════════════════════════════════════════════════════════ */

/* This function has unreachable code after return */
static int dce_example(int x)
{
    if (x > 0)
        return x;
    else
        return -x;

    /* Dead code — never reached.  The compiler removes it at -O2. */
    printf("This line is dead code!\n");
    return 0;
}

static void demo_dead_code_elimination(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Dead Code Elimination (DCE)                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("DCE removes code that can never execute or whose result\n");
    printf("is never used.\n\n");

    printf("  Example 1 — Unreachable code:\n");
    printf("    return x;\n");
    printf("    printf(\"dead!\");  ← removed by compiler\n\n");

    printf("  Example 2 — Unused variable:\n");
    printf("    int unused = expensive_function();  ← removed if pure\n\n");

    printf("  Example 3 — Constant condition:\n");
    printf("    if (0) { ... }   ← entire block removed\n\n");

    printf("  Demo: dce_example(5)  = %d\n", dce_example(5));
    printf("  Demo: dce_example(-3) = %d\n", dce_example(-3));
    printf("  (The dead printf was never executed.)\n\n");

    printf("  With -Wall, GCC warns:\n");
    printf("    warning: will never be executed [-Wunreachable-code]\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Strength Reduction
 * ════════════════════════════════════════════════════════════════════ */
static void demo_strength_reduction(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Strength Reduction                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Strength reduction replaces expensive operations with\n");
    printf("cheaper equivalents.\n\n");

    printf("  ┌────────────────────────┬─────────────────────────────┐\n");
    printf("  │ Before (expensive)     │ After (cheap)               │\n");
    printf("  ├────────────────────────┼─────────────────────────────┤\n");
    printf("  │ x * 2                  │ x << 1                      │\n");
    printf("  │ x * 8                  │ x << 3                      │\n");
    printf("  │ x / 4                  │ x >> 2  (unsigned)          │\n");
    printf("  │ x %% 8                 │ x & 7   (power-of-2)        │\n");
    printf("  │ x * 15                 │ (x<<4) - x                  │\n");
    printf("  └────────────────────────┴─────────────────────────────┘\n\n");

    int x = 42;
    printf("  Demo: x = %d\n", x);
    printf("    x * 8  = %d   (compiler uses: x << 3)\n", x * 8);
    printf("    x / 4  = %d   (compiler uses: x >> 2)\n", x / 4);
    printf("    x %% 8  = %d    (compiler uses: x & 7)\n", x % 8);
    printf("\n");

    printf("  Also applies inside loops:\n");
    printf("    for (i = 0; i < n; i++)\n");
    printf("        arr[i] = ...;         ← arr[i] = *(arr + i*4)\n");
    printf("    becomes:\n");
    printf("    p = arr;\n");
    printf("    for (i = 0; i < n; i++, p++)\n");
    printf("        *p = ...;             ← increment pointer by 4\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Loop Optimisations
 * ════════════════════════════════════════════════════════════════════ */
static void demo_loop_optimisations(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Loop Optimisations                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Loops consume most execution time, so they get special treatment.\n\n");

    printf("  1. Loop-Invariant Code Motion (LICM):\n");
    printf("     Move computations that don't change between iterations\n");
    printf("     outside the loop.\n");
    printf("       Before: for (i=0; i<n; i++) a[i] = x * y + i;\n");
    printf("       After:  t = x * y;\n");
    printf("               for (i=0; i<n; i++) a[i] = t + i;\n\n");

    printf("  2. Loop Unrolling:\n");
    printf("     Reduce loop overhead by duplicating the body.\n");
    printf("       Before: for (i=0; i<4; i++) sum += a[i];\n");
    printf("       After:  sum += a[0]; sum += a[1]; sum += a[2]; sum += a[3];\n\n");

    printf("  3. Loop Vectorisation (-O3 or -ftree-vectorize):\n");
    printf("     Process multiple array elements per instruction (SIMD).\n");
    printf("       for (i=0; i<n; i++) c[i] = a[i] + b[i];\n");
    printf("     Can use SSE/AVX to process 4 or 8 floats at once.\n\n");

    printf("  4. Loop Interchange (for nested loops):\n");
    printf("     Reorder loops for better cache locality.\n");
    printf("       Before: for(j) for(i) a[i][j]    ← column-major, cache-hostile\n");
    printf("       After:  for(i) for(j) a[i][j]    ← row-major, cache-friendly\n\n");

    /* Demonstrate unrolling */
    int arr[] = {10, 20, 30, 40, 50, 60, 70, 80};
    int sum_loop = 0, sum_unrolled = 0;
    for (int i = 0; i < 8; i++) sum_loop += arr[i];
    /* Manual unroll */
    sum_unrolled = arr[0]+arr[1]+arr[2]+arr[3]+arr[4]+arr[5]+arr[6]+arr[7];
    printf("  Demo: sum(loop)=%d, sum(unrolled)=%d  (same result)\n\n", sum_loop, sum_unrolled);

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  See which optimisations GCC applies:                   ║\n");
    printf("║  gcc -O2 -fopt-info-optimized optimisation.c -o /dev/null║\n");
    printf("║  gcc -O3 -fopt-info-vec optimisation.c -o /dev/null     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Function Inlining
 * ════════════════════════════════════════════════════════════════════ */

/* Small function — likely inlined at -O2 */
static inline int square(int x) { return x * x; }

static void demo_inlining(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Function Inlining                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Inlining replaces a function call with the function body.\n\n");

    printf("  Before:  y = square(x);     ← call overhead       \n");
    printf("  After:   y = x * x;         ← no call, no return  \n\n");

    printf("  Benefits:\n");
    printf("    - Eliminates call/return overhead\n");
    printf("    - Enables further optimisations (constant fold, etc.)\n");
    printf("  Costs:\n");
    printf("    - Increases code size (bloat)\n");
    printf("    - May hurt instruction cache if overdone\n\n");

    printf("  Demo: square(7) = %d\n", square(7));
    printf("  Demo: square(12) = %d\n", square(12));
    printf("  (At -O2, no 'call' instruction — body is inlined.)\n\n");

    printf("  GCC hints:\n");
    printf("    inline int f(int x) { ... }       ← suggest inlining\n");
    printf("    __attribute__((always_inline))     ← force inlining\n");
    printf("    __attribute__((noinline))          ← prevent inlining\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Common Subexpression Elimination (CSE)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_cse(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Common Subexpression Elimination (CSE)     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("CSE detects repeated expressions and computes them once.\n\n");

    printf("  Before:\n");
    printf("    a = b + c;\n");
    printf("    d = b + c + e;   ← 'b + c' computed again\n\n");
    printf("  After:\n");
    printf("    t1 = b + c;\n");
    printf("    a = t1;\n");
    printf("    d = t1 + e;      ← reuse previous result\n\n");

    /* Demo CSE with array indexing */
    int arr[5] = {10, 20, 30, 40, 50};
    int idx = 2;
    int val1 = arr[idx] + 1;     /* arr[idx] computed here */
    int val2 = arr[idx] * 2;     /* arr[idx] reused via CSE */
    printf("  Demo: arr[%d]+1 = %d, arr[%d]*2 = %d\n", idx, val1, idx, val2);
    printf("  (The address calculation arr+idx*4 is done once.)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Tail Call Optimisation
 * ════════════════════════════════════════════════════════════════════ */

/* Tail-recursive function — compiler can turn into a loop */
static int factorial_tail(int n, int acc)
{
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);  /* tail position */
}

static void demo_tail_call(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Tail Call Optimisation                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When the last action of a function is a call, the compiler\n");
    printf("can reuse the current stack frame → no stack growth.\n\n");

    printf("  Tail-recursive factorial:\n");
    printf("    int fact(int n, int acc) {\n");
    printf("        if (n <= 1) return acc;\n");
    printf("        return fact(n-1, n*acc);  ← tail call\n");
    printf("    }\n\n");
    printf("  At -O2, GCC turns this into a loop (no recursive calls).\n\n");

    printf("  Demo: factorial_tail(10, 1) = %d\n", factorial_tail(10, 1));
    printf("  Demo: factorial_tail(12, 1) = %d\n\n", factorial_tail(12, 1));

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Verify: gcc -O2 -S optimisation.c                      ║\n");
    printf("║  Search for factorial_tail — no 'call' instruction,     ║\n");
    printf("║  just conditional jump back to the top.                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 9 — Summary of Optimisations
 * ════════════════════════════════════════════════════════════════════ */
static void demo_summary(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 9 — Summary                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌───────────────────────────────┬──────┬──────┬──────┐\n");
    printf("  │ Optimisation                  │ -O1  │ -O2  │ -O3  │\n");
    printf("  ├───────────────────────────────┼──────┼──────┼──────┤\n");
    printf("  │ Constant folding              │  Y   │  Y   │  Y   │\n");
    printf("  │ Dead code elimination         │  Y   │  Y   │  Y   │\n");
    printf("  │ Common subexpr elimination    │  Y   │  Y   │  Y   │\n");
    printf("  │ Strength reduction            │  Y   │  Y   │  Y   │\n");
    printf("  │ Function inlining             │  -   │  Y   │  Y+  │\n");
    printf("  │ Loop-invariant code motion    │  Y   │  Y   │  Y   │\n");
    printf("  │ Loop unrolling                │  -   │  -   │  Y   │\n");
    printf("  │ Vectorisation (SIMD)          │  -   │  -   │  Y   │\n");
    printf("  │ Tail call optimisation        │  -   │  Y   │  Y   │\n");
    printf("  └───────────────────────────────┴──────┴──────┴──────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 22 — Optimisation Passes                           ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_why_optimise();
    demo_constant_folding();
    demo_dead_code_elimination();
    demo_strength_reduction();
    demo_loop_optimisations();
    demo_inlining();
    demo_cse();
    demo_tail_call();
    demo_summary();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 22 — Optimisation Passes\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
