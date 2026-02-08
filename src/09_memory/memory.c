/*
 * Chapter 9 — Dynamic Memory Management
 *
 * The heap is your canvas — malloc gives you memory, free returns it.
 * Get it wrong and you get crashes, leaks, or worse.  This chapter:
 *   1. malloc — allocating raw bytes
 *   2. calloc — zero-initialized allocation
 *   3. realloc — growing and shrinking buffers
 *   4. free — releasing memory, NULL idiom
 *   5. Memory layout — stack vs heap addresses
 *   6. Common bugs — use-after-free, leaks, off-by-one
 *   7. Valgrind patterns — writing valgrind-clean code
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/09_memory \
 *            src/09_memory/memory.c
 * Run:   ./bin/09_memory
 *        valgrind --leak-check=full ./bin/09_memory
 *
 * Try these:
 *   - Comment out a free() call and run with valgrind
 *   - Change realloc size to 0 — what does your system do?
 *   - Compile with -fsanitize=address and trigger a use-after-free
 *   - Check: does calloc(SIZE_MAX, SIZE_MAX) return NULL?
 */

#include "../../include/common.h"

/* Forward-declare main so we can print its address in demo_memory_layout */
int main(void);

/* ════════════════════════════════════════════════════════════════
 *  Section 1: malloc — Basic Heap Allocation
 *  Allocates uninitialized memory from the heap.
 * ════════════════════════════════════════════════════════════════ */

static void demo_malloc(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: malloc — Basic Heap Allocation         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* malloc returns void*; returns NULL on failure                  */
    int *arr = malloc(5 * sizeof(int));
    if (!arr) {
        perror("malloc");      /* ALWAYS check the return value      */
        return;
    }

    /* Memory is UNINITIALIZED — could be anything                   */
    printf("  malloc(5 * sizeof(int)) = %p\n", (void *)arr);
    printf("  Uninitialized values:");
    for (int i = 0; i < 5; i++) {
        arr[i] = (i + 1) * 10;
        printf(" %d", arr[i]);
    }
    printf("  (we wrote these)\n\n");

    /* The casting debate:                                           */
    printf("  To cast or not to cast?\n");
    printf("    int *p = malloc(n);          // C: implicit void* → int* is fine\n");
    printf("    int *p = (int*)malloc(n);    // C++: required, harmless in C\n");
    printf("  Rule: in pure C, don't cast (can hide missing <stdlib.h>).\n\n");

    /* sizeof idiom: use the variable, not the type                  */
    printf("  Best practice: malloc(count * sizeof(*arr))\n");
    printf("  → if arr's type changes, sizeof adjusts automatically.\n\n");

    free(arr);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: calloc — Zero-Initialized Allocation
 *  Like malloc, but zeroes every byte.
 * ════════════════════════════════════════════════════════════════ */

static void demo_calloc(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: calloc — Zero-Initialized Allocation   ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* calloc(count, size) — two arguments, not one!                 */
    int *arr = calloc(5, sizeof(int));
    if (!arr) { perror("calloc"); return; }

    printf("  calloc(5, sizeof(int)) guarantees all zeros:\n  ");
    for (int i = 0; i < 5; i++)
        printf(" arr[%d]=%d", i, arr[i]);  /* all 0 */
    printf("\n\n");

    /* calloc also checks for multiplication overflow internally     */
    printf("  calloc vs malloc:\n");
    printf("    malloc(n * size)  → n*size can OVERFLOW silently!\n");
    printf("    calloc(n, size)   → checks overflow, returns NULL.\n");
    printf("  Use calloc when you need zeroed memory or large arrays.\n\n");

    free(arr);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: realloc — Resize Existing Allocation
 *  Grow or shrink a buffer without losing data.
 * ════════════════════════════════════════════════════════════════ */

static void demo_realloc(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: realloc — Resize Allocation            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int *buf = malloc(3 * sizeof(int));
    if (!buf) { perror("malloc"); return; }
    buf[0] = 10; buf[1] = 20; buf[2] = 30;

    printf("  Original (3 ints): %d %d %d  at %p\n", buf[0], buf[1], buf[2], (void *)buf);

    /* realloc may move the data to a new address!                   */
    int *new_buf = realloc(buf, 6 * sizeof(int));
    if (!new_buf) {
        /* CRITICAL: if realloc fails, the original pointer is STILL VALID */
        perror("realloc");
        free(buf);             /* must free the original             */
        return;
    }
    buf = new_buf;             /* always reassign                    */
    buf[3] = 40; buf[4] = 50; buf[5] = 60;

    printf("  Grown (6 ints):    ");
    for (int i = 0; i < 6; i++) printf("%d ", buf[i]);
    printf(" at %p\n\n", (void *)buf);

    /* Handy equivalences                                            */
    printf("  realloc(NULL, n)  == malloc(n)   (allocate fresh)\n");
    printf("  realloc(ptr,  0)  → implementation-defined (don't rely on it)\n\n");

    /* THE CARDINAL SIN: never do this:                              */
    printf("  NEVER:  buf = realloc(buf, new_size);\n");
    printf("  If realloc fails, buf becomes NULL → old memory leaked!\n");
    printf("  ALWAYS: tmp = realloc(buf, new_size); if (tmp) buf = tmp;\n\n");

    free(buf);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: free — Releasing Memory
 *  The matching bookend to every malloc/calloc/realloc.
 * ════════════════════════════════════════════════════════════════ */

static void demo_free(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: free — Releasing Memory                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int *p = malloc(sizeof(int));
    *p = 42;
    printf("  Before free: *p = %d, p = %p\n", *p, (void *)p);

    free(p);
    /* p still holds the old address — it's now a "dangling pointer" */
    printf("  After free:  p = %p  (still the same address!)\n", (void *)p);

    p = NULL;                  /* defensive: makes p safe to check   */
    printf("  After p=NULL: p = %p  (safe sentinel value)\n\n", (void *)p);

    /* free(NULL) is always safe — defined as a no-op                */
    free(NULL);
    printf("  free(NULL) → safe no-op (C standard guarantees this)\n\n");

    /* The double-free problem                                       */
    printf("  Double free danger:\n");
    printf("    free(p); free(p);  → heap corruption, crash, exploit!\n");
    printf("    Setting p = NULL after free prevents this:\n");
    printf("    free(p); p = NULL; free(p);  → free(NULL) = safe.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Memory Layout — Stack vs Heap
 *  Observing where different variables live in memory.
 * ════════════════════════════════════════════════════════════════ */

static void demo_memory_layout(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Memory Layout — Stack vs Heap          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Stack variables: allocated automatically, fast, limited       */
    int stack_var = 100;
    int stack_arr[4] = {1, 2, 3, 4};

    /* Heap variables: allocated manually, slower, much larger       */
    int *heap_var = malloc(sizeof(int));
    int *heap_arr = malloc(4 * sizeof(int));
    *heap_var = 200;

    /* Static/global: live for entire program                        */
    static int static_var = 300;

    printf("  Variable       Address            Region\n");
    printf("  ─────────────  ─────────────────  ──────\n");
    printf("  stack_var      %p   stack\n",  (void *)&stack_var);
    printf("  stack_arr      %p   stack\n",  (void *)stack_arr);
    printf("  heap_var       %p   heap\n",   (void *)heap_var);
    printf("  heap_arr       %p   heap\n",   (void *)heap_arr);
    printf("  static_var     %p   data/bss\n", (void *)&static_var);
    printf("  main           %p   text\n\n", (void *)main);

    printf("  What happens: stack grows downward (high → low addresses)\n");
    printf("  while the heap grows upward (low → high).  They grow\n");
    printf("  toward each other — if they meet, you're out of memory.\n\n");

    free(heap_var);
    free(heap_arr);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Common Memory Bugs
 *  The bugs that keep C programmers up at night.
 * ════════════════════════════════════════════════════════════════ */

static void demo_common_bugs(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Common Memory Bugs                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* 1. Use after free (conceptual — don't actually do this!)      */
    printf("  1) Use-after-free:\n");
    printf("     int *p = malloc(sizeof(int));\n");
    printf("     free(p);\n");
    printf("     *p = 42;  // writes to freed memory → UB!\n");
    printf("     Symptom: works sometimes, crashes randomly later.\n\n");

    /* 2. Memory leak — allocate and forget                          */
    printf("  2) Memory leak:\n");
    printf("     void leaky() {\n");
    printf("         char *buf = malloc(1024);\n");
    printf("         if (error) return;  // buf is leaked!\n");
    printf("     }\n");
    printf("     Long-running programs: slow death by memory exhaustion.\n\n");

    /* 3. Off-by-one in allocation                                   */
    printf("  3) Off-by-one:\n");
    char *str = malloc(5);     /* need 6 for \"Hello\" + '\\0'        */
    if (str) {
        strncpy(str, "Hello", 5);
        /* str[5] is the '\0' but we only allocated 5 bytes!         */
        printf("     malloc(5) for \"Hello\" → need 6 (forgot '\\0'!)\n");
        printf("     Fix: malloc(strlen(\"Hello\") + 1)\n\n");
        free(str);
    }

    /* 4. Uninitialized read from malloc                             */
    printf("  4) Reading uninitialized heap memory:\n");
    printf("     int *x = malloc(sizeof(int));\n");
    printf("     printf(\"%%d\", *x);  // value is garbage!\n");
    printf("     Fix: use calloc, or initialize immediately.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Valgrind Patterns
 *  Writing code that passes valgrind --leak-check=full cleanly.
 * ════════════════════════════════════════════════════════════════ */

static void demo_valgrind_patterns(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Valgrind Patterns                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  Valgrind detects at runtime:\n");
    printf("    • Memory leaks (allocated but never freed)\n");
    printf("    • Use of uninitialized values\n");
    printf("    • Read/write after free\n");
    printf("    • Read/write past allocation bounds\n");
    printf("    • Double free / mismatched free\n\n");

    /* Pattern: single-owner allocation                              */
    printf("  Pattern 1: Single-owner (claim, use, release):\n");
    int *data = malloc(10 * sizeof(int));
    if (data) {
        for (int i = 0; i < 10; i++) data[i] = i * i;
        printf("    Squares: ");
        for (int i = 0; i < 10; i++) printf("%d ", data[i]);
        printf("\n");
        free(data);
        data = NULL;           /* immediately NULL after free        */
    }

    /* Pattern: error-path cleanup with goto                         */
    printf("\n  Pattern 2: goto cleanup (multiple allocations):\n");
    printf("    int *a = malloc(n); if (!a) goto fail;\n");
    printf("    int *b = malloc(n); if (!b) goto fail_a;\n");
    printf("    // use a and b ...\n");
    printf("    free(b); free(a); return OK;\n");
    printf("    fail_a: free(a);\n");
    printf("    fail:   return ERROR;\n\n");

    /* How to run valgrind                                           */
    printf("  Run:  valgrind --leak-check=full --show-reachable=yes \\\n");
    printf("                 --track-origins=yes ./bin/09_memory\n\n");
    printf("  Goal: \"All heap blocks were freed -- no leaks are possible\"\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main — run all demos in order
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 9 — Dynamic Memory          v%s        ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════╝\n");

    demo_malloc();
    demo_calloc();
    demo_realloc();
    demo_free();
    demo_memory_layout();
    demo_common_bugs();
    demo_valgrind_patterns();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: Every malloc needs a free.  Every pointer\n");
    printf(" needs an owner.  Check returns, NULL after free, and\n");
    printf(" let valgrind be your safety net.\n");
    printf("════════════════════════════════════════════════════════\n");

    DEMO_END();
    return 0;
}
