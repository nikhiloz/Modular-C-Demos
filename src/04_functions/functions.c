/**
 * @file functions.c
 * @brief C functions: declarations, definitions, recursion, variadic, callbacks
 */
#include "../../include/common.h"
#include <stdarg.h>

/* Forward declarations */
int add(int a, int b);
void swap_by_value(int a, int b);
void swap_by_ref(int *a, int *b);
int factorial(int n);
int fibonacci(int n);

/* Basic function */
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }

/* Pass by value vs reference */
void swap_by_value(int a, int b) { int t = a; a = b; b = t; }
void swap_by_ref(int *a, int *b) { int t = *a; *a = *b; *b = t; }

/* Recursion */
int factorial(int n) { return (n <= 1) ? 1 : n * factorial(n - 1); }
int fibonacci(int n) { return (n <= 1) ? n : fibonacci(n-1) + fibonacci(n-2); }

/* Static function (file scope only) */
static int helper_func(int x) { return x * 2; }

/* Inline function */
static inline int square(int x) { return x * x; }

/* Variadic function */
int sum_all(int count, ...) {
    va_list args;
    va_start(args, count);
    int sum = 0;
    for (int i = 0; i < count; i++) sum += va_arg(args, int);
    va_end(args);
    return sum;
}

/* Function pointer */
typedef int (*math_op)(int, int);
int apply_op(int a, int b, math_op op) { return op(a, b); }

/* Callback pattern */
void foreach(int *arr, int n, void (*callback)(int)) {
    for (int i = 0; i < n; i++) callback(arr[i]);
}
void print_item(int x) { printf("%d ", x); }
void print_squared(int x) { printf("%d ", x*x); }

void demo_basic_functions(void) {
    DEMO_SECTION("Basic Functions");
    printf("add(3, 4) = %d\n", add(3, 4));
    printf("multiply(5, 6) = %d\n", multiply(5, 6));
    printf("helper_func(10) = %d (static)\n", helper_func(10));
    printf("square(7) = %d (inline)\n", square(7));
}

void demo_pass_by(void) {
    DEMO_SECTION("Pass by Value vs Reference");
    int a = 5, b = 10;
    printf("Before: a=%d, b=%d\n", a, b);
    swap_by_value(a, b);
    printf("After swap_by_value: a=%d, b=%d (unchanged)\n", a, b);
    swap_by_ref(&a, &b);
    printf("After swap_by_ref: a=%d, b=%d (swapped)\n", a, b);
}

void demo_recursion(void) {
    DEMO_SECTION("Recursion");
    printf("Factorial: ");
    for (int i = 0; i <= 6; i++) printf("%d!=%d ", i, factorial(i));
    printf("\n");
    printf("Fibonacci: ");
    for (int i = 0; i <= 10; i++) printf("%d ", fibonacci(i));
    printf("\n");
}

void demo_variadic(void) {
    DEMO_SECTION("Variadic Functions");
    printf("sum_all(3, 10,20,30) = %d\n", sum_all(3, 10, 20, 30));
    printf("sum_all(5, 1,2,3,4,5) = %d\n", sum_all(5, 1, 2, 3, 4, 5));
}

void demo_function_pointers(void) {
    DEMO_SECTION("Function Pointers");
    math_op op = add;
    printf("op(3,4) with add = %d\n", op(3, 4));
    op = multiply;
    printf("op(3,4) with multiply = %d\n", op(3, 4));
    printf("apply_op(5,6,add) = %d\n", apply_op(5, 6, add));
    printf("apply_op(5,6,multiply) = %d\n", apply_op(5, 6, multiply));
}

void demo_callbacks(void) {
    DEMO_SECTION("Callbacks");
    int arr[] = {1, 2, 3, 4, 5};
    printf("foreach print: "); foreach(arr, 5, print_item); printf("\n");
    printf("foreach squared: "); foreach(arr, 5, print_squared); printf("\n");
}

int main(void) {
    printf("\n=== C FUNCTIONS DEMO v%s ===\n", VERSION_STRING);
    demo_basic_functions();
    demo_pass_by();
    demo_recursion();
    demo_variadic();
    demo_function_pointers();
    demo_callbacks();
    DEMO_END(); return 0;
}
