/**
 * @file preprocessor.c
 * @brief C preprocessor: macros, conditionals, pragmas
 */
#include "../../include/common.h"

/* Object-like macros */
#define PI 3.14159
#define MAX_SIZE 100

/* Function-like macros */
#define SQUARE(x) ((x) * (x))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define PRINT_VAR(x) printf(#x " = %d\n", x)

/* Variadic macro */
#define DEBUG_LOG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

/* Stringification and concatenation */
#define STR(x) #x
#define CONCAT(a,b) a##b

/* Multi-line macro */
#define SWAP_INT(a,b) do { \
    int _t = a; a = b; b = _t; \
} while(0)

void demo_object_macros(void) {
    DEMO_SECTION("Object-like Macros");
    printf("PI = %f\n", PI);
    printf("MAX_SIZE = %d\n", MAX_SIZE);
    printf("VERSION_STRING = %s\n", VERSION_STRING);
}

void demo_function_macros(void) {
    DEMO_SECTION("Function-like Macros");
    printf("SQUARE(5) = %d\n", SQUARE(5));
    printf("MAX(10, 20) = %d\n", MAX(10, 20));
    int value = 42;
    PRINT_VAR(value);
    DEBUG_LOG("Test message with %d", 123);
}

void demo_stringify_concat(void) {
    DEMO_SECTION("Stringify and Concatenation");
    printf("STR(hello) = %s\n", STR(hello));
    int var1 = 10, var2 = 20;
    printf("CONCAT(var,1) = %d\n", CONCAT(var,1));
    printf("CONCAT(var,2) = %d\n", CONCAT(var,2));
}

void demo_predefined(void) {
    DEMO_SECTION("Predefined Macros");
    printf("__FILE__ = %s\n", __FILE__);
    printf("__LINE__ = %d\n", __LINE__);
    printf("__DATE__ = %s\n", __DATE__);
    printf("__TIME__ = %s\n", __TIME__);
    printf("__func__ = %s\n", __func__);
#ifdef __STDC_VERSION__
    printf("__STDC_VERSION__ = %ld\n", __STDC_VERSION__);
#endif
}

void demo_conditionals(void) {
    DEMO_SECTION("Conditional Compilation");
#if defined(__linux__) || defined(__ANDROID__)
    printf("Platform: Linux/Android\n");
#elif defined(_WIN32)
    printf("Platform: Windows\n");
#else
    printf("Platform: Unknown\n");
#endif

#ifdef DEBUG
    printf("DEBUG mode enabled\n");
#else
    printf("DEBUG mode disabled\n");
#endif

#if MAX_SIZE > 50
    printf("MAX_SIZE > 50\n");
#endif
}

void demo_swap_macro(void) {
    DEMO_SECTION("Multi-line Macro");
    int a = 5, b = 10;
    printf("Before: a=%d, b=%d\n", a, b);
    SWAP_INT(a, b);
    printf("After SWAP_INT: a=%d, b=%d\n", a, b);
}

int main(void) {
    printf("\n=== C PREPROCESSOR DEMO v%s ===\n", VERSION_STRING);
    demo_object_macros();
    demo_function_macros();
    demo_stringify_concat();
    demo_predefined();
    demo_conditionals();
    demo_swap_macro();
    DEMO_END(); return 0;
}
