/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 5 — C Arrays                                            ║
 * ║  Modular-C-Demos                                                 ║
 * ║  Topics: 1D/2D arrays, array decay, VLAs, passing arrays to      ║
 * ║          functions, common pitfalls, sizeof behaviour              ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 *  Build:  make 05_arrays
 *     or:  gcc -std=c11 -Wall -Wextra -o arrays arrays.c
 *
 *  Try these:
 *    1. Change arr[5] access to arr[10] — no runtime error, just garbage
 *    2. Declare a VLA with size 10000000 — watch the stack overflow
 *    3. Pass an array to a function and sizeof it there — surprise!
 *    4. Remove {} initialiser and read the array — uninitialised garbage
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Basic Arrays
 * ════════════════════════════════════════════════════════════════════ */
static void demo_basic_arrays(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Basic Arrays                                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Declaration and initialisation — array size is fixed at compile
     * time (unless VLA, see Section 4).  Elements are contiguous. */
    int arr[5] = {10, 20, 30, 40, 50};

    printf("  Declaration: int arr[5] = {10, 20, 30, 40, 50};\n\n");

    /* Access: zero-indexed.  arr[i] is syntactic sugar for *(arr + i). */
    printf("  Access:  ");
    for (int i = 0; i < 5; i++)
        printf("arr[%d]=%d  ", i, arr[i]);
    printf("\n\n");

    /* sizeof on an array gives TOTAL BYTES, not element count.
     * Use ARRAY_SIZE() macro: sizeof(arr) / sizeof(arr[0]) */
    printf("  sizeof(arr)    = %zu  (total bytes: 5 ints × %zu bytes)\n",
           sizeof(arr), sizeof(int));
    printf("  sizeof(arr[0]) = %zu  (one element)\n", sizeof(arr[0]));
    printf("  ARRAY_SIZE(arr)= %zu  (element count via macro)\n\n", ARRAY_SIZE(arr));

    /* Partial initialisation: remaining elements are zero-filled */
    int partial[5] = {1, 2};            /* → {1, 2, 0, 0, 0} */
    printf("  Partial init {1,2}: ");
    for (int i = 0; i < 5; i++) printf("%d ", partial[i]);
    printf(" (rest zero-filled)\n");

    /* Size inferred from initialiser list */
    int inferred[] = {7, 8, 9};         /* Compiler deduces size = 3 */
    printf("  Inferred size: %zu elements\n", ARRAY_SIZE(inferred));
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Multidimensional Arrays
 * ════════════════════════════════════════════════════════════════════ */
static void demo_multidimensional(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Multidimensional Arrays                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 2D array: stored in ROW-MAJOR order (row 0, then row 1, ...).
     * matrix[r][c] is at offset (r * COLS + c) from the base address.
     * This matters enormously for cache performance. */
    int matrix[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    printf("  3×4 matrix (row-major layout):\n");
    for (int r = 0; r < 3; r++) {
        printf("    ");
        for (int c = 0; c < 4; c++)
            printf("%3d ", matrix[r][c]);
        printf("\n");
    }

    /* Memory layout proof: consecutive elements */
    printf("\n  Memory layout (contiguous, row by row):\n    ");
    int *flat = &matrix[0][0];          /* Treat as flat 1D array */
    for (int i = 0; i < 12; i++)
        printf("%d ", flat[i]);
    printf("\n\n");

    printf("  sizeof(matrix) = %zu  (3 rows × 4 cols × %zu bytes)\n",
           sizeof(matrix), sizeof(int));

    printf("\n  What happens: Accessing matrix[row][col] computes\n");
    printf("  *(base + row*4 + col).  Iterating row-first (inner loop\n");
    printf("  over cols) is cache-friendly; column-first is not.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Array Decay to Pointer
 * ════════════════════════════════════════════════════════════════════ */
static void demo_array_decay(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Array Decay to Pointer                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int arr[5] = {10, 20, 30, 40, 50};

    /* In most contexts, an array name "decays" to a pointer to its
     * first element.  This is NOT a conversion — the array is not
     * a pointer.  It just BEHAVES like one in expressions. */
    int *p = arr;                       /* arr decays to &arr[0] */

    printf("  arr (decayed) = %p\n", (void*)arr);
    printf("  &arr[0]       = %p  (same address)\n", (void*)&arr[0]);
    printf("  p             = %p  (pointer copy)\n\n", (void*)p);

    /* The crucial difference: sizeof */
    printf("  sizeof(arr) = %zu  (total array size — 5 × %zu)\n",
           sizeof(arr), sizeof(int));
    printf("  sizeof(p)   = %zu  (pointer size — always %zu on this platform)\n\n",
           sizeof(p), sizeof(void*));

    /* Three exceptions where arrays do NOT decay:
     *   1. sizeof(arr)     — gives total bytes
     *   2. &arr            — gives pointer-to-array, not pointer-to-element
     *   3. String literal initialiser for char[] */
    printf("  &arr  type: int(*)[5]  — pointer to ENTIRE array\n");
    printf("  &arr  addr: %p\n", (void*)&arr);
    printf("  &arr+1 addr: %p  (jumps by %zu bytes = entire array)\n",
           (void*)(&arr + 1), sizeof(arr));
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Variable-Length Arrays (C99)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_vla(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Variable-Length Arrays (C99)                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* VLA: size determined at RUNTIME, allocated on the STACK.
     * NOT resizable — once created, the size is fixed.
     * Optional in C11 (check __STDC_NO_VLA__), mandatory in C99. */
    int n = 6;
    int vla[n];                         /* Stack-allocated, size = n */

    for (int i = 0; i < n; i++)
        vla[i] = i * i;

    printf("  VLA of size %d: ", n);
    for (int i = 0; i < n; i++)
        printf("%d ", vla[i]);
    printf("\n\n");

    /* sizeof on a VLA is evaluated at RUNTIME (unlike normal arrays) */
    printf("  sizeof(vla) = %zu  (runtime evaluation: %d × %zu)\n\n",
           sizeof(vla), n, sizeof(int));

    printf("  Dangers of VLAs:\n");
    printf("    - Stack-allocated: large n → stack overflow (no warning!)\n");
    printf("    - Cannot be initialised with { } syntax\n");
    printf("    - Cannot be static or extern\n");
    printf("    - The Linux kernel bans VLAs (CONFIG_CC_NO_STACKPROTECTOR)\n");
    printf("    - Prefer malloc() for runtime-sized allocations\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Passing Arrays to Functions
 * ════════════════════════════════════════════════════════════════════ */

/* When you write "int arr[]" in a parameter list, it is IDENTICAL to
 * "int *arr".  The array decays — size information is LOST.  You must
 * always pass the size as a separate parameter. */
static void print_array(const int arr[], int size)
{
    /* arr is a pointer here — sizeof(arr) == sizeof(int*), NOT the
     * total array size.  This is the #1 sizeof pitfall in C. */
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
}

/* For 2D arrays, the compiler needs the column count to compute
 * row offsets.  The first dimension can be omitted. */
static void print_matrix(int rows, int cols, const int mat[rows][cols])
{
    for (int r = 0; r < rows; r++) {
        printf("    ");
        for (int c = 0; c < cols; c++)
            printf("%3d ", mat[r][c]);
        printf("\n");
    }
}

static void demo_array_as_param(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Passing Arrays to Functions                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int arr[] = {5, 10, 15, 20, 25};
    int size = (int)ARRAY_SIZE(arr);

    printf("  Passed to print_array(): ");
    print_array(arr, size);
    printf("\n\n");

    printf("  Inside the function, sizeof(arr) = sizeof(int*) = %zu\n", sizeof(int*));
    printf("  The array size is LOST — always pass size separately!\n\n");

    /* 2D array parameter */
    int mat[2][3] = {{1, 2, 3}, {4, 5, 6}};
    printf("  2D array passed to function:\n");
    print_matrix(2, 3, mat);

    printf("\n  What happens: void f(int arr[100]) is the same as\n");
    printf("  void f(int *arr).  The '100' is ignored by the compiler!\n");
    printf("  Use void f(int arr[static 100]) (C99) to promise ≥100 elements.\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Common Pitfalls
 * ════════════════════════════════════════════════════════════════════ */
static void demo_common_pitfalls(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Common Pitfalls                              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Pitfall 1: Out-of-bounds access — C performs NO runtime bounds
     * checking.  Reading/writing past the end is undefined behaviour.
     * It may work, crash, or corrupt unrelated data silently. */
    int arr[5] = {10, 20, 30, 40, 50};
    printf("  Pitfall 1: Out-of-bounds access\n");
    printf("    arr[5] would read past the end — undefined behaviour!\n");
    printf("    C has NO runtime bounds checking (unlike Java, Python)\n");
    printf("    Use ASAN (-fsanitize=address) to catch these.\n\n");

    /* Pitfall 2: Uninitialised arrays — local arrays without an
     * initialiser contain whatever garbage was on the stack. */
    printf("  Pitfall 2: Uninitialised local arrays contain garbage\n");
    printf("    int x[3]; — contents are indeterminate (stack leftovers)\n");
    printf("    static int x[3]; — zero-initialised (in .bss segment)\n\n");

    /* Pitfall 3: sizeof after decay */
    printf("  Pitfall 3: sizeof after array-to-pointer decay\n");
    printf("    int arr[100]; sizeof(arr) = %zu  (correct: total bytes)\n",
           100 * sizeof(int));
    printf("    void f(int arr[]) { sizeof(arr) } = %zu  (WRONG: ptr size)\n\n",
           sizeof(int*));

    /* Pitfall 4: Returning a local array */
    printf("  Pitfall 4: Returning a pointer to a local array\n");
    printf("    int* f() { int arr[5]; return arr; } ← DANGLING POINTER\n");
    printf("    The array is on the stack and is destroyed when f() returns.\n");
    printf("    Use malloc() or pass a buffer as parameter instead.\n\n");

    /* Demonstration with ARRAY_SIZE macro — safe pattern */
    printf("  Safe pattern: always pair arrays with ARRAY_SIZE():\n");
    printf("    for (int i = 0; i < (int)ARRAY_SIZE(arr); i++)\n");
    printf("    Elements: ");
    for (int i = 0; i < (int)ARRAY_SIZE(arr); i++)
        printf("%d ", arr[i]);
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 5 — C Arrays  v%s                              ║\n",
           VERSION_STRING);
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_basic_arrays();
    demo_multidimensional();
    demo_array_decay();
    demo_vla();
    demo_array_as_param();
    demo_common_pitfalls();

    DEMO_END();
    return 0;
}
