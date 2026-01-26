/**
 * @file data_types.c
 * @brief Comprehensive demonstration of C data types
 */
#include "../../include/common.h"

void demo_integer_types(void) {
    DEMO_SECTION("Integer Data Types");
    char c = 65; signed char sc = -128; unsigned char uc = 255;
    printf("char: %c (size: %zu), signed char: %d, unsigned char: %u\n", c, sizeof(c), sc, uc);
    short s = -32768; unsigned short us = 65535;
    printf("short: %hd (size: %zu), unsigned short: %hu\n", s, sizeof(s), us);
    int i = -2147483647; unsigned int ui = 4294967295U;
    printf("int: %d (size: %zu), unsigned int: %u\n", i, sizeof(i), ui);
    long l = -2147483648L; unsigned long ul = 4294967295UL;
    printf("long: %ld (size: %zu), unsigned long: %lu\n", l, sizeof(l), ul);
    long long ll = -9223372036854775807LL; unsigned long long ull = 18446744073709551615ULL;
    printf("long long: %lld (size: %zu)\n", ll, sizeof(ll));
}

void demo_floating_types(void) {
    DEMO_SECTION("Floating Point Types");
    float f = 3.14159265f; double d = 3.141592653589793; long double ld = 3.14159265358979323846L;
    printf("float: %.7f (size: %zu, precision: %d)\n", f, sizeof(f), FLT_DIG);
    printf("double: %.15lf (size: %zu, precision: %d)\n", d, sizeof(d), DBL_DIG);
    printf("long double: %.18Lf (size: %zu)\n", ld, sizeof(ld));
    printf("Limits: FLT_MAX=%e, DBL_MAX=%e\n", FLT_MAX, DBL_MAX);
}

void demo_fixed_width_types(void) {
    DEMO_SECTION("Fixed-Width Integer Types (stdint.h)");
    printf("int8: %d, int16: %d, int32: %d\n", INT8_MIN, INT16_MIN, INT32_MIN);
    printf("uint8: %u, uint16: %u, uint32: %u\n", UINT8_MAX, UINT16_MAX, UINT32_MAX);
    printf("size_t: %zu bytes, ptrdiff_t: %zu bytes\n", sizeof(size_t), sizeof(ptrdiff_t));
}

void demo_type_qualifiers(void) {
    DEMO_SECTION("Type Qualifiers");
    const int readonly = 100; printf("const: %d\n", readonly);
    volatile int hw_reg = 42; printf("volatile: %d\n", hw_reg);
    int arr[5] = {1,2,3,4,5}; int * restrict p = arr; printf("restrict: %d\n", *p);
}

void demo_storage_classes(void) {
    DEMO_SECTION("Storage Classes");
    auto int a = 1; printf("auto: %d\n", a);
    static int cnt = 0; cnt++; printf("static: %d\n", cnt);
    register int r = 42; printf("register: %d\n", r);
}

void demo_derived_types(void) {
    DEMO_SECTION("Derived Types");
    int arr[5] = {1,2,3,4,5}; printf("Array size: %zu\n", sizeof(arr));
    int *ptr = arr; printf("Pointer: %p -> %d\n", (void*)ptr, *ptr);
    enum Color { RED=1, GREEN=2, BLUE=4 }; enum Color c = GREEN;
    printf("Enum: %d (size: %zu)\n", c, sizeof(c));
}

void demo_boolean_type(void) {
    DEMO_SECTION("Boolean Type (C99)");
    bool t = true, f = false; printf("true=%d, false=%d, size=%zu\n", t, f, sizeof(t));
}

void demo_void_type(void) {
    DEMO_SECTION("Void Type");
    int x = 42; void *gp = &x; printf("void* -> int: %d\n", *(int*)gp);
}

int main(void) {
    printf("\n=== C DATA TYPES DEMO v%s ===\n", VERSION_STRING);
    demo_integer_types(); demo_floating_types(); demo_fixed_width_types();
    demo_type_qualifiers(); demo_storage_classes(); demo_derived_types();
    demo_boolean_type(); demo_void_type();
    DEMO_END(); return 0;
}
