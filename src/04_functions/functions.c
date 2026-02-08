/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 4 — C Functions                                         ║
 * ║  Modular-C-Demos                                                 ║
 * ║  Topics: declarations, definitions, pass-by-value/reference,     ║
 * ║          recursion, variadic, function pointers, callbacks,       ║
 * ║          stack frames, calling conventions                        ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 *  Build:  make 04_functions
 *     or:  gcc -std=c11 -Wall -Wextra -o functions functions.c
 *
 *  Try these:
 *    1. Call factorial(20) — it overflows i32 but not i64
 *    2. Call fibonacci(40) — watch exponential time blow up
 *    3. Pass a NULL function pointer and watch the segfault
 *    4. Compile with -S and inspect the calling convention (registers)
 */

#include "../../include/common.h"
#include <stdarg.h>

/* ════════════════════════════════════════════════════════════════════
 *  Forward Declarations
 *  The compiler processes top-to-bottom.  A forward declaration tells
 *  it "this function exists, here's its signature" so it can type-check
 *  calls before seeing the full definition.
 * ════════════════════════════════════════════════════════════════════ */
int add(int a, int b);
void swap_by_value(int a, int b);
void swap_by_ref(int *a, int *b);
int factorial(int n);
int fibonacci(int n);

/* ════════════════════════════════════════════════════════════════════
 *  Function Definitions
 * ════════════════════════════════════════════════════════════════════ */

/* Basic functions — return type, name, parameter list */
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }

/* static: limits visibility to THIS file only (internal linkage).
 * This is how you make "private" functions in C. */
static int helper_func(int x) { return x * 2; }

/* inline: a HINT to the compiler to expand the function body at the
 * call site, avoiding the overhead of a function call.  Modern
 * compilers decide this themselves at -O2 regardless of the keyword. */
static inline int square(int x) { return x * x; }

/* ════════════════════════════════════════════════════════════════════
 *  Pass by Value vs Reference
 * ════════════════════════════════════════════════════════════════════ */

/* Pass by value: C copies the ACTUAL VALUE into the parameter.
 * The function works on a copy — the original is untouched.
 * This is the ONLY parameter passing mode in C. */
void swap_by_value(int a, int b)
{
    int t = a; a = b; b = t;            /* Swaps local copies only */
}

/* "Pass by reference" in C: we pass a POINTER (by value!).
 * The function receives a copy of the address — but that address
 * still points to the original data, so we can modify it. */
void swap_by_ref(int *a, int *b)
{
    int t = *a; *a = *b; *b = t;        /* Dereference to reach original */
}

/* ════════════════════════════════════════════════════════════════════
 *  Recursion
 *  Each recursive call pushes a new stack frame (~dozens of bytes).
 *  Deep recursion can overflow the stack (default ~8 MB on Linux).
 * ════════════════════════════════════════════════════════════════════ */
int factorial(int n)
{
    /* Base case prevents infinite recursion */
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

int fibonacci(int n)
{
    /* Naive recursive fibonacci: O(2^n) time, O(n) stack depth.
     * Each call spawns two more — exponential explosion.
     * For production, use iterative or memoised version. */
    return (n <= 1) ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

/* ════════════════════════════════════════════════════════════════════
 *  Variadic Functions
 *  Uses <stdarg.h> macros:
 *    va_list  — type holding the argument pointer
 *    va_start — initialise, pointing past the last fixed arg
 *    va_arg   — retrieve next argument (YOU must know the type)
 *    va_end   — cleanup (required by the standard)
 * ════════════════════════════════════════════════════════════════════ */
int sum_all(int count, ...)
{
    va_list args;
    va_start(args, count);              /* Start after 'count' */
    int sum = 0;
    for (int i = 0; i < count; i++)
        sum += va_arg(args, int);       /* Pop next int from the stack */
    va_end(args);                       /* Always pair with va_start */
    return sum;
}

/* ════════════════════════════════════════════════════════════════════
 *  Function Pointers & Callbacks
 *
 *  Syntax breakdown:  int (*op)(int, int)
 *                     ^^^  ^^  ^^^^^^^^^
 *                     |    |   parameter types
 *                     |    pointer-to-function name
 *                     return type
 *
 *  The parentheses around (*op) are critical — without them,
 *  int *op(int, int)  declares a function returning int*.
 * ════════════════════════════════════════════════════════════════════ */
typedef int (*math_op)(int, int);       /* Typedef cleans up the syntax */

int apply_op(int a, int b, math_op op)
{
    return op(a, b);                    /* Call through the pointer */
}

/* Callback pattern: pass a function to iterate over data.
 * This is the C equivalent of higher-order functions. */
void foreach(int *arr, int n, void (*callback)(int))
{
    for (int i = 0; i < n; i++)
        callback(arr[i]);
}

void print_item(int x) { printf("%d ", x); }
void print_squared(int x) { printf("%d ", x * x); }

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Basic Functions Demo
 * ════════════════════════════════════════════════════════════════════ */
static void demo_basic_functions(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Basic Functions                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  add(3, 4)      = %d\n", add(3, 4));
    printf("  multiply(5, 6) = %d\n", multiply(5, 6));
    printf("  helper_func(10)= %d  (static — file-private)\n", helper_func(10));
    printf("  square(7)      = %d  (inline — expanded at call site)\n\n", square(7));

    printf("  What happens: When you call add(3, 4), the compiler:\n");
    printf("  1. Pushes arguments onto the stack (or into registers)\n");
    printf("  2. Saves the return address\n");
    printf("  3. Jumps to the function body\n");
    printf("  4. Executes, places return value in %%eax (x86)\n");
    printf("  5. Pops the stack frame and returns to the caller\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Pass by Value vs Reference
 * ════════════════════════════════════════════════════════════════════ */
static void demo_pass_by(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Pass by Value vs Reference                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int a = 5, b = 10;
    printf("  Before:           a = %d, b = %d\n", a, b);

    swap_by_value(a, b);
    printf("  After by-value:   a = %d, b = %d  (unchanged!)\n", a, b);

    swap_by_ref(&a, &b);
    printf("  After by-ref:     a = %d, b = %d  (swapped)\n\n", a, b);

    printf("  What happens: swap_by_value receives COPIES of a and b.\n");
    printf("  It swaps the copies, which are destroyed when the function\n");
    printf("  returns.  swap_by_ref receives POINTERS to the originals,\n");
    printf("  so dereferencing (*a, *b) modifies the caller's variables.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Recursion
 * ════════════════════════════════════════════════════════════════════ */
static void demo_recursion(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Recursion                                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Factorial: ");
    for (int i = 0; i <= 6; i++)
        printf("%d!=%d  ", i, factorial(i));
    printf("\n\n");

    printf("  Fibonacci: ");
    for (int i = 0; i <= 10; i++)
        printf("%d ", fibonacci(i));
    printf("\n\n");

    printf("  What happens: Each call to factorial() creates a new stack\n");
    printf("  frame.  factorial(5) → 5 frames deep.  factorial(100000)\n");
    printf("  would overflow the stack (~8 MB default on Linux).\n");
    printf("  Tail-call optimisation (-O2) can reuse the frame, but only\n");
    printf("  if the recursive call is the LAST operation.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Variadic Functions
 * ════════════════════════════════════════════════════════════════════ */
static void demo_variadic(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Variadic Functions                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  sum_all(3, 10,20,30)  = %d\n", sum_all(3, 10, 20, 30));
    printf("  sum_all(5, 1,2,3,4,5) = %d\n\n", sum_all(5, 1, 2, 3, 4, 5));

    printf("  How va_list works internally (x86-64 System V ABI):\n");
    printf("    - First 6 integer args go in: rdi, rsi, rdx, rcx, r8, r9\n");
    printf("    - Additional args spill to the stack\n");
    printf("    - va_start sets up a pointer to the first variadic arg\n");
    printf("    - va_arg advances the pointer by sizeof(type)\n");
    printf("    - If you pass the wrong type to va_arg → undefined behaviour\n");
    printf("    - printf() relies on format string to know argument types\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Function Pointers
 * ════════════════════════════════════════════════════════════════════ */
static void demo_function_pointers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Function Pointers                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* A function pointer holds the address of executable code.
     * The function name alone (without parens) decays to a pointer. */
    math_op op = add;                   /* &add also works */
    printf("  op = add;       op(3,4) = %d\n", op(3, 4));

    op = multiply;                      /* Reassign to different function */
    printf("  op = multiply;  op(3,4) = %d\n\n", op(3, 4));

    /* apply_op demonstrates the strategy pattern — the algorithm
     * (which operation) is chosen at runtime via function pointer. */
    printf("  apply_op(5, 6, add)      = %d\n", apply_op(5, 6, add));
    printf("  apply_op(5, 6, multiply) = %d\n\n", apply_op(5, 6, multiply));

    printf("  Syntax breakdown:\n");
    printf("    int (*op)(int, int)  — pointer to func returning int\n");
    printf("    int  *op (int, int)  — func returning int* (different!)\n");
    printf("    typedef cleans this up: typedef int (*math_op)(int, int);\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Callbacks
 * ════════════════════════════════════════════════════════════════════ */
static void demo_callbacks(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Callbacks                                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int arr[] = {1, 2, 3, 4, 5};
    int n = (int)ARRAY_SIZE(arr);

    printf("  foreach(arr, print_item):    ");
    foreach(arr, n, print_item);
    printf("\n");

    printf("  foreach(arr, print_squared): ");
    foreach(arr, n, print_squared);
    printf("\n\n");

    printf("  What happens: foreach() knows nothing about printing —\n");
    printf("  it just calls whatever function you hand it.  This is the\n");
    printf("  foundation of event-driven programming in C (signal handlers,\n");
    printf("  qsort comparators, pthread creation, GUI toolkit events).\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 4 — C Functions  v%s                           ║\n",
           VERSION_STRING);
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_basic_functions();
    demo_pass_by();
    demo_recursion();
    demo_variadic();
    demo_function_pointers();
    demo_callbacks();

    DEMO_END();
    return 0;
}
