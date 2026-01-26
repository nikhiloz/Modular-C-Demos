#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Bit macros (safer: parenthesized, use unsigned shift) */
#define CLR_BIT(x, n) ((x) &= ~((1U) << (n)))
#define SET_BIT(x, n) ((x) |= ((1U) << (n)))

/* malloc wrapper with error checking */
static void *malloc_t(size_t size)
{
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* Simple arithmetic helpers */
static int sum(int x, int y)  { return x + y; }
static int diff(int x, int y) { return x - y; }

/* allocate an int and return pointer (caller must free) */
static int *pass_func(int val)
{
    int *k = malloc_t(sizeof *k);
    *k = val;
    return k;
}

int main(int argc, char **argv)
{
    printf("Hello\n");

    /* Arguments */
    printf("Argument count = %d\n", argc);
    for (int i = 0; i < argc; ++i)
        printf("argv[%d] = %s\n", i, argv[i]);

    /* Array of function pointers */
    typedef int (*func_arr_t)(int, int);
    func_arr_t func_arr[] = { sum, diff };
    printf("%d\n", func_arr[0](1, 4));
    printf("%d\n", func_arr[1](8, 4));

    /* Bit operations */
    int i = 224;
    printf("%d\n", i);
    CLR_BIT(i, 5);
    printf("%d\n", i);
    SET_BIT(i, 5);
    printf("%d\n", i);

    /* Malloc wrapper usage (remember to free) */
    int *p = pass_func(150);
    printf("%d\n", ++(*p));
    free(p);

    /* Endianness check */
    int n = 1;
    if (*(char *)&n == 1)
        printf("Machine is Little Endian\n");
    else
        printf("Machine is Big Endian\n");

    /* Bit-field structure (use unsigned for bit-fields) */
    struct str_t {
        unsigned int a1:1;
        unsigned int a2:2;
        unsigned int a3:5;
    };

    struct str_t st = {0, 1, 15};
    printf("st.a1 = %u\n", st.a1);
    printf("st.a2 = %u\n", st.a2);
    printf("st.a3 = %u\n", st.a3);

    /* Array of string literals */
    char *ptrs[] = { "Nikhilesh", "Rajiv", "Manoj" };
    printf("%s\n", ptrs[0]);

    /* Char array and pointer */
    char name[] = "Hello This is me\n";
    char *c = name;
    printf("%s", c);

    /* Ternary expression printing */
    int n1 = 10;
    printf("%s\n", (n1 < 10) ? "YES" : "NO");

    return 0;
}