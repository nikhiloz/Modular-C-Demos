/*
 * Chapter 6 — Pointers
 *
 * The single most important concept in C.  Pointers let you:
 *   1. Refer to data indirectly (address-of & dereference)
 *   2. Walk through arrays efficiently (pointer arithmetic)
 *   3. Share and modify data across functions (pass-by-pointer)
 *   4. Build dynamic data structures (linked lists, trees)
 *   5. Implement callbacks (function pointers)
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/06_pointers \
 *            src/06_pointers/pointers.c
 * Run:   ./bin/06_pointers
 *
 * Try these:
 *   - Change `int x` to `double x` — what changes in the pointer?
 *   - Print sizeof(int*) vs sizeof(double*) — are they the same?
 *   - Uncomment the dangling-pointer dereference — what happens?
 *   - Compile with -fsanitize=address and trigger use-after-free
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Pointer Basics
 *  Declaring, dereferencing, address-of, and NULL.
 * ════════════════════════════════════════════════════════════════ */

static void demo_basics(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Pointer Basics                         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int x = 42;
    int *p = &x;               /* & = "address-of" operator           */

    printf("  x   = %d\n", x);
    printf("  &x  = %p          ← address of x on the stack\n", (void *)&x);
    printf("  p   = %p          ← pointer holds same address\n", (void *)p);
    printf("  *p  = %d              ← dereference: follow the arrow\n\n", *p);

    *p = 99;                   /* modify x through the pointer        */
    printf("  After *p = 99:  x = %d   (x changed — same memory!)\n\n", x);

    int *null_ptr = NULL;      /* NULL = address 0, means "points nowhere" */
    printf("  NULL pointer value: %p\n", (void *)null_ptr);
    printf("  Dereferencing NULL → segfault (don't do it!)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Pointer Arithmetic
 *  Adding/subtracting integers, pointer difference, stride.
 * ════════════════════════════════════════════════════════════════ */

static void demo_arithmetic(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Pointer Arithmetic                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int arr[] = {10, 20, 30, 40, 50};
    int *p = arr;              /* array name decays to pointer to [0] */

    /* p + n advances by n * sizeof(int) bytes, not n bytes!         */
    printf("  arr = {10, 20, 30, 40, 50}\n");
    for (int i = 0; i < 5; i++) {
        printf("  *(p + %d) = %d   addr = %p\n", i, *(p + i), (void *)(p + i));
    }

    /* Pointer difference gives element count, not byte count        */
    int *start = &arr[0];
    int *end   = &arr[4];
    printf("\n  end - start = %td elements  (not bytes!)\n", end - start);
    printf("  Byte difference = %td\n", (char *)end - (char *)start);
    printf("  sizeof(int) = %zu  → %td * %zu = %td bytes ✓\n\n",
           sizeof(int), end - start, sizeof(int),
           (end - start) * (long)sizeof(int));
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Pointers and Arrays
 *  Array decay, equivalence of ptr[i] and *(ptr+i), sizeof trap.
 * ════════════════════════════════════════════════════════════════ */

static void demo_and_arrays(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Pointers and Arrays                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int arr[5] = {1, 2, 3, 4, 5};
    int *p = arr;              /* "decay": array name → pointer to first element */

    /* These four expressions all access the same element:           */
    printf("  arr[2]     = %d\n", arr[2]);
    printf("  *(arr + 2) = %d\n", *(arr + 2));
    printf("  p[2]       = %d\n", p[2]);
    printf("  *(p + 2)   = %d\n", *(p + 2));
    printf("  2[arr]     = %d   ← legal but weird (a[i] == *(a+i) == *(i+a) == i[a])\n\n", 2[arr]);

    /* The critical difference: sizeof                               */
    printf("  sizeof(arr) = %zu   ← full array: 5 × %zu bytes\n", sizeof(arr), sizeof(int));
    printf("  sizeof(p)   = %zu     ← just a pointer (always %zu on this machine)\n\n",
           sizeof(p), sizeof(void *));

    printf("  What happens: when you pass an array to a function,\n");
    printf("  it decays to a pointer — you lose the size info.\n");
    printf("  That's why functions need a separate 'length' parameter.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Pointer-to-Pointer (Double Pointer)
 *  Why you need ** to modify a pointer from another function.
 * ════════════════════════════════════════════════════════════════ */

/* This function modifies where the caller's pointer points         */
static void allocate_array(int **pp, int n)
{
    /* *pp dereferences the double pointer → accesses caller's ptr   */
    *pp = malloc(n * sizeof(int));
    if (*pp) {
        for (int i = 0; i < n; i++)
            (*pp)[i] = (i + 1) * 100;
    }
}

static void demo_double_pointer(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Pointer-to-Pointer (Double Pointer)    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int x = 10;
    int *p  = &x;
    int **pp = &p;             /* pp points to p, p points to x      */

    printf("  x   = %d\n", x);
    printf("  *p  = %d    (one dereference: p → x)\n", *p);
    printf("  **pp = %d    (two dereferences: pp → p → x)\n\n", **pp);

    /* Practical use: function that allocates memory for the caller  */
    int *data = NULL;
    allocate_array(&data, 4); /* pass &data so function can modify data */
    printf("  allocate_array(&data, 4) →");
    for (int i = 0; i < 4; i++)
        printf(" %d", data[i]);
    printf("\n");
    printf("  Without **, the function could only modify a local copy.\n\n");
    free(data);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Void Pointers
 *  The generic "any type" pointer — must cast before dereference.
 * ════════════════════════════════════════════════════════════════ */

static void print_value(const void *data, char type)
{
    /* void* can hold any pointer, but you must cast to dereference  */
    switch (type) {
        case 'i': printf("  int:    %d\n",   *(const int *)data);    break;
        case 'f': printf("  float:  %.2f\n", *(const float *)data);  break;
        case 'c': printf("  char:   '%c'\n", *(const char *)data);   break;
    }
}

static void demo_void_pointer(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Void Pointers                          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    int   i = 42;
    float f = 3.14f;
    char  c = 'Z';

    /* void* erases type information — you become responsible        */
    printf("  void* can point to any type (used by malloc, qsort, etc.):\n");
    print_value(&i, 'i');
    print_value(&f, 'f');
    print_value(&c, 'c');

    /* malloc returns void* — that's why it works with any type      */
    void *raw = malloc(sizeof(int));
    *(int *)raw = 777;         /* must cast before writing           */
    printf("\n  malloc returns void*: *(int*)raw = %d\n", *(int *)raw);
    printf("  You can't do arithmetic on void* (size unknown).\n\n");
    free(raw);
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Function Pointers
 *  Syntax, typedef trick, and the callback pattern.
 * ════════════════════════════════════════════════════════════════ */

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }

/* typedef makes function pointer types readable                    */
typedef int (*binop_t)(int, int);

/* A function that takes a callback — the "strategy" pattern        */
static void apply_and_print(int x, int y, binop_t op, const char *name)
{
    printf("  %s(%d, %d) = %d\n", name, x, y, op(x, y));
}

static void demo_function_pointers(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Function Pointers                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Raw syntax:  int (*fp)(int, int) = add;                       */
    int (*fp)(int, int) = add;
    printf("  Raw declaration: int (*fp)(int, int)\n");
    printf("  fp(10, 3) = %d\n\n", fp(10, 3));

    /* With typedef it's much cleaner:                               */
    printf("  With typedef 'binop_t':\n");
    apply_and_print(10, 3, add, "add");
    apply_and_print(10, 3, sub, "sub");
    apply_and_print(10, 3, mul, "mul");

    /* Array of function pointers — dispatch table                   */
    binop_t ops[] = {add, sub, mul};
    const char *names[] = {"add", "sub", "mul"};
    printf("\n  Dispatch table (array of function pointers):\n");
    for (size_t i = 0; i < ARRAY_SIZE(ops); i++) {
        printf("    ops[%zu](7, 2) = %d  [%s]\n", i, ops[i](7, 2), names[i]);
    }
    printf("\n  What happens: qsort() uses a function pointer for its\n");
    printf("  comparator — that's the classic callback pattern in C.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Common Pointer Pitfalls
 *  Dangling, wild, and double-free — the bugs that ruin your day.
 * ════════════════════════════════════════════════════════════════ */

static void demo_common_pitfalls(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Common Pointer Pitfalls                ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* 1. Dangling pointer: pointer to freed memory                  */
    printf("  1) Dangling pointer (use-after-free):\n");
    int *p = malloc(sizeof(int));
    *p = 42;
    printf("     Before free: *p = %d\n", *p);
    free(p);
    /* *p is now dangling — dereferencing is undefined behavior!      */
    printf("     After free: p still holds %p but memory is invalid.\n", (void *)p);
    p = NULL;                  /* ALWAYS set to NULL after free       */
    printf("     Fix: set p = NULL after free → safe to check.\n\n");

    /* 2. Wild pointer: uninitialized pointer                        */
    printf("  2) Wild pointer (uninitialized):\n");
    printf("     int *wild;  // points to random address!\n");
    printf("     *wild = 5;  // writes to unknown memory → crash or corruption\n");
    printf("     Fix: always initialize: int *safe = NULL;\n\n");

    /* 3. Double free                                                */
    printf("  3) Double free:\n");
    printf("     free(p); free(p);  // undefined behavior!\n");
    printf("     Heap metadata corruption → crashes, security exploits.\n");
    printf("     Fix: set pointer to NULL after free; free(NULL) is safe.\n\n");

    /* 4. Memory leak                                                */
    printf("  4) Memory leak:\n");
    printf("     int *leak = malloc(100);\n");
    printf("     leak = malloc(200);  // old 100 bytes lost forever!\n");
    printf("     Fix: free(old) before reassigning the pointer.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  main — run all demos in order
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 6 — Pointers               v%s        ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════╝\n");

    demo_basics();
    demo_arithmetic();
    demo_and_arrays();
    demo_double_pointer();
    demo_void_pointer();
    demo_function_pointers();
    demo_common_pitfalls();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: A pointer is just an address.  Master the\n");
    printf(" five flavours (data, array, double, void, function)\n");
    printf(" and you can build anything in C.\n");
    printf("════════════════════════════════════════════════════════\n");

    DEMO_END();
    return 0;
}
