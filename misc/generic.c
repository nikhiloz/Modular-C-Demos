#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Bit macros: clear and set a bit at position 'n' in variable 'x'.
 * Using 1U ensures an unsigned shift, and parenthesized operands
 * avoid surprises with operator precedence.
 */
#define CLR_BIT(x, n) ((x) &= ~((1U) << (n)))
#define SET_BIT(x, n) ((x) |= ((1U) << (n)))

/*
 * malloc_t: lightweight wrapper around malloc that checks for allocation
 * failure. Exits the program on failure after printing an error.
 */
static void *malloc_t(size_t size)
{
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* Simple arithmetic helper functions used by the function-pointer demo. */
static int sum(int x, int y)  { return x + y; }
static int diff(int x, int y) { return x - y; }

/*
 * pass_func: allocate an int on the heap, initialize it with 'val',
 * and return the pointer. Caller is responsible for freeing the pointer.
 */
static int *pass_func(int val)
{
    int *k = malloc_t(sizeof *k); /* allocate enough memory for one int */
    *k = val;
    return k;
}

/*
 * Example main demonstrating various small C idioms and behaviors:
 * - argument printing
 * - array of function pointers
 * - bit manipulation macros
 * - heap allocation wrapper usage
 * - endianness check
 * - bit-field usage
 * - arrays of string literals and character arrays
 * - ternary operator usage
 */
int main(int argc, char **argv)
{
    printf("Hello\n");

    /* Print argument count and each argument (argv[0] is the program name). */
    printf("Argument count = %d\n", argc);
    for (int i = 0; i < argc; ++i)
        printf("argv[%d] = %s\n", i, argv[i]);

    /*
     * Array of function pointers: demonstrates storing and calling functions
     * with the same signature via a pointer array.
     */
    typedef int (*func_arr_t)(int, int);
    func_arr_t func_arr[] = { sum, diff };
    printf("%d\n", func_arr[0](1, 4)); /* calls sum(1,4) */
    printf("%d\n", func_arr[1](8, 4)); /* calls diff(8,4) */

    /* Bit operations demo using the CLR_BIT/SET_BIT macros. */
    int i = 224;
    printf("%d\n", i);
    CLR_BIT(i, 5); /* clear bit 5 */
    printf("%d\n", i);
    SET_BIT(i, 5); /* set bit 5 back */
    printf("%d\n", i);

    /* Demonstrate allocating an int on the heap and modifying it. */
    int *p = pass_func(150);
    printf("%d\n", ++(*p)); /* increment the stored value and print */
    free(p); /* free the heap allocation to avoid a memory leak */

    /* Endianness check: examine the lowest-address byte of an int. */
    int n = 1;
    if (*(char *)&n == 1)
        printf("Machine is Little Endian\n");
    else
        printf("Machine is Big Endian\n");

    /*
     * Bit-field structure: defines several fields with specific bit-widths.
     * Use unsigned types for bit-fields unless signed behavior is required.
     */
    struct str_t {
        unsigned int a1:1; /* 1-bit field */
        unsigned int a2:2; /* 2-bit field */
        unsigned int a3:5; /* 5-bit field */
    };

    /* Initialize the bit-field struct using a compound literal initializer. */
    struct str_t st = {0, 1, 15};
    printf("st.a1 = %u\n", st.a1);
    printf("st.a2 = %u\n", st.a2);
    printf("st.a3 = %u\n", st.a3);

    /* Array of string literals: pointers to constant strings in read-only memory. */
    char *ptrs[] = { "Nikhilesh", "Rajiv", "Manoj" };
    printf("%s\n", ptrs[0]);

    /* Char array and pointer: shows that arrays decay to pointers. */
    char name[] = "Hello This is me\n";
    char *c = name;
    printf("%s", c);

    /* Ternary operator example used inside printf. */
    int n1 = 10;
    printf("%s\n", (n1 < 10) ? "YES" : "NO");

    return 0;
}