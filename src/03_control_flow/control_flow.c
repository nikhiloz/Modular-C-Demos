/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 3 — C Control Flow                                      ║
 * ║  Modular-C-Demos                                                 ║
 * ║  Topics: if/else, switch, for, while, do-while, break/continue,  ║
 * ║          goto, Duff's device, infinite loops, fall-through        ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 *  Build:  make 03_control_flow
 *     or:  gcc -std=c11 -Wall -Wextra -o control_flow control_flow.c
 *
 *  Try these:
 *    1. Remove a 'break' from the switch and watch fall-through happen
 *    2. Replace for(;;) with while(1) — both are idiomatic infinite loops
 *    3. Add -Wimplicit-fallthrough to catch missing break in switch
 *    4. Try nesting goto labels — note: can't jump INTO a VLA scope
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — If-Else Statements
 * ════════════════════════════════════════════════════════════════════ */
static void demo_if_else(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — If-Else Statements                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int x = 15;

    /* Classic if-else chain — evaluated top-to-bottom, first true wins.
     * The braces are optional for single statements, but ALWAYS use them
     * to avoid the "dangling else" bug. */
    if (x > 20) {
        printf("  x > 20\n");
    } else if (x > 10) {
        printf("  10 < x <= 20  (x = %d)\n", x);
    } else {
        printf("  x <= 10\n");
    }

    /* Ternary operator: compact if-else that returns a value.
     * Don't nest these — readability drops fast. */
    int a = 5, b = 10;
    int max_val = (a > b) ? a : b;
    printf("  Ternary: max(%d, %d) = %d\n\n", a, b, max_val);

    /* What happens: In C, the condition is any expression.  Zero is
     * false, everything else is true.  There is no dedicated boolean
     * comparison — if(ptr) is idiomatic for "if not NULL". */
    printf("  Truthiness: if(42) → true, if(0) → false, if(ptr) → null check\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Switch Statement
 * ════════════════════════════════════════════════════════════════════ */
static void demo_switch(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Switch Statement                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int day = 3;

    /* Switch compiles to a jump table when cases are dense — O(1) lookup
     * instead of O(n) if-else chain.  Cases must be integer constants. */
    switch (day) {
        case 1: printf("  Monday\n");    break;
        case 2: printf("  Tuesday\n");   break;
        case 3: printf("  Wednesday\n"); break;

        /* Fall-through: multiple cases sharing one body.
         * This is intentional — add a comment to suppress warnings. */
        case 6: /* fall through */
        case 7: printf("  Weekend\n");   break;

        default: printf("  Other day\n");
    }

    /* What happens: Without 'break', execution FALLS THROUGH to the
     * next case.  This is a famous source of bugs.  Modern compilers
     * warn about it with -Wimplicit-fallthrough.
     *
     * GCC __attribute__((fallthrough)) or a comment "fall through"
     * silences the warning when fall-through is intentional. */
    printf("\n  Fall-through demo (day=1, no break after case 1):\n");
    printf("  Would print: Monday Tuesday Wednesday ... (all cases!)\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — For Loops
 * ════════════════════════════════════════════════════════════════════ */
static void demo_for_loop(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — For Loops                                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Standard for loop: init; condition; increment.
     * C99 allows declaring the loop variable inside for(). */
    printf("  Basic:   ");
    for (int i = 0; i < 5; i++) printf("%d ", i);
    printf("\n");

    printf("  Step 2:  ");
    for (int i = 0; i < 10; i += 2) printf("%d ", i);
    printf("\n");

    /* Nested loops — the multiplication table pattern */
    printf("  Nested 3×3:\n");
    for (int i = 1; i <= 3; i++) {
        printf("    ");
        for (int j = 1; j <= 3; j++)
            printf("%2d ", i * j);
        printf("\n");
    }

    /* Infinite loop idioms — both are common, for(;;) is slightly
     * more "traditional" in systems code (Linux kernel uses it). */
    printf("\n  Infinite loop idioms:  for(;;) { }  or  while(1) { }\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — While and Do-While Loops
 * ════════════════════════════════════════════════════════════════════ */
static void demo_while_loops(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — While and Do-While Loops                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* while: checks condition BEFORE each iteration.
     * If the condition is false initially, the body never executes. */
    int n = 5;
    printf("  While countdown: ");
    while (n > 0) printf("%d ", n--);
    printf("Liftoff!\n");

    /* do-while: checks condition AFTER each iteration.
     * The body always runs at least once — useful for input validation
     * loops and macro wrappers (do { ... } while(0) idiom). */
    n = 0;
    printf("  Do-while (n=0): ");
    do { printf("%d ", n++); } while (n < 3);
    printf("\n");

    /* Key difference: do-while with a false condition still runs once */
    n = 10;
    printf("  Do-while false:  ");
    do { printf("%d ", n); } while (n < 5);
    printf(" (runs once even though 10 < 5 is false)\n\n");

    printf("  What happens: The do { ... } while(0) pattern wraps multi-\n");
    printf("  statement macros safely — it acts as a single statement\n");
    printf("  and requires a semicolon after the call.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Break and Continue
 * ════════════════════════════════════════════════════════════════════ */
static void demo_break_continue(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Break and Continue                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* continue: skip the rest of THIS iteration, jump to the next.
     * In a for loop, the increment still runs after continue. */
    printf("  Continue (skip even): ");
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) continue;      /* Skip even numbers */
        printf("%d ", i);
    }
    printf("\n");

    /* break: exit the INNERMOST enclosing loop or switch immediately.
     * To break out of nested loops, use goto or a flag variable. */
    printf("  Break at i==5:        ");
    for (int i = 0; i < 10; i++) {
        if (i == 5) break;
        printf("%d ", i);
    }
    printf("\n\n");

    /* Breaking out of nested loops — goto is the clean solution */
    printf("  Nested break with goto:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 1 && j == 1) goto found;
            printf("    (%d,%d) ", i, j);
        }
        printf("\n");
    }
found:
    printf("\n    Found at (1,1) — goto jumped out of both loops\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Goto (Error Cleanup Pattern)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_goto(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Goto (Error Cleanup Pattern)                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* goto is almost universally avoided EXCEPT for one pattern:
     * centralised error cleanup in functions that acquire multiple
     * resources.  This pattern is used extensively in the Linux kernel.
     *
     * The alternative (deeply nested if-else) is harder to read and
     * more error-prone when you add another resource. */

    int *buf1 = malloc(100 * sizeof(int));
    if (!buf1) goto fail_buf1;

    int *buf2 = malloc(200 * sizeof(int));
    if (!buf2) goto fail_buf2;

    /* Success path — use both buffers */
    buf1[0] = 42;
    buf2[0] = 84;
    printf("  Allocated buf1[0]=%d, buf2[0]=%d\n", buf1[0], buf2[0]);
    printf("  Both resources acquired — cleanup in reverse order\n\n");

    /* Cleanup in reverse order of acquisition (stack-like) */
    free(buf2);
fail_buf2:
    free(buf1);
fail_buf1:
    printf("  Cleanup complete\n\n");

    printf("  What happens: The Linux kernel uses this pattern thousands\n");
    printf("  of times.  Each goto label frees resources acquired up to\n");
    printf("  that point.  It's the C equivalent of RAII destructors.\n");
    printf("  Rule: goto should only jump FORWARD, within the same function.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Duff's Device (Curiosity)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_duffs_device(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Duff's Device (Historical Curiosity)        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Duff's device: an optimisation trick that interleaves a switch
     * with a do-while loop for loop unrolling.  It exploits C's
     * fall-through semantics.  Rarely used today — compilers unroll
     * loops better than humans. */
    int src[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int dst[10] = {0};
    int count = 10;
    int n = (count + 7) / 8;            /* Ceiling division */
    int i = 0;

    switch (count % 8) {
        case 0: do { dst[i] = src[i]; i++;  /* fall through */
        case 7:      dst[i] = src[i]; i++;  /* fall through */
        case 6:      dst[i] = src[i]; i++;  /* fall through */
        case 5:      dst[i] = src[i]; i++;  /* fall through */
        case 4:      dst[i] = src[i]; i++;  /* fall through */
        case 3:      dst[i] = src[i]; i++;  /* fall through */
        case 2:      dst[i] = src[i]; i++;  /* fall through */
        case 1:      dst[i] = src[i]; i++;
                } while (--n > 0);
    }

    printf("  Copied %d elements via Duff's device: ", count);
    for (int k = 0; k < count; k++) printf("%d ", dst[k]);
    printf("\n\n");

    printf("  What happens: The switch jumps into the middle of a\n");
    printf("  do-while loop to handle the remainder, then the loop\n");
    printf("  copies 8 elements per iteration.  Fun, but let the\n");
    printf("  compiler handle loop unrolling with -O2 instead.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 3 — C Control Flow  v%s                        ║\n",
           VERSION_STRING);
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_if_else();
    demo_switch();
    demo_for_loop();
    demo_while_loops();
    demo_break_continue();
    demo_goto();
    demo_duffs_device();

    DEMO_END();
    return 0;
}
