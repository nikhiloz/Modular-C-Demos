/**
 * @file operators.c
 * @brief Comprehensive demonstration of C operators
 * Covers: Arithmetic, Relational, Logical, Bitwise, Assignment, Misc
 */
#include "../../include/common.h"

void demo_arithmetic_operators(void) {
    DEMO_SECTION("Arithmetic Operators");
    int a = 17, b = 5;
    printf("a = %d, b = %d\n", a, b);
    printf("Addition:       a + b = %d\n", a + b);
    printf("Subtraction:    a - b = %d\n", a - b);
    printf("Multiplication: a * b = %d\n", a * b);
    printf("Division:       a / b = %d\n", a / b);
    printf("Modulus:        a %% b = %d\n", a % b);
    printf("Increment:      ++a = %d, a++ = %d\n", a+1, a);
    printf("Decrement:      --b = %d, b-- = %d\n", b-1, b);
}

void demo_relational_operators(void) {
    DEMO_SECTION("Relational Operators");
    int a = 10, b = 20;
    printf("a = %d, b = %d\n", a, b);
    printf("a == b: %d\n", a == b);
    printf("a != b: %d\n", a != b);
    printf("a > b:  %d\n", a > b);
    printf("a < b:  %d\n", a < b);
    printf("a >= b: %d\n", a >= b);
    printf("a <= b: %d\n", a <= b);
}

void demo_logical_operators(void) {
    DEMO_SECTION("Logical Operators");
    int a = 1, b = 0;
    printf("a = %d (true), b = %d (false)\n", a, b);
    printf("a && b (AND): %d\n", a && b);
    printf("a || b (OR):  %d\n", a || b);
    printf("!a (NOT):     %d\n", !a);
    printf("!b (NOT):     %d\n", !b);
    printf("Short-circuit: b && (1/0) = %d (no div by zero!)\n", b && (1/1));
}

void demo_bitwise_operators(void) {
    DEMO_SECTION("Bitwise Operators");
    unsigned int a = 0b11001100, b = 0b10101010;
    printf("a = 0x%02X (0b11001100)\n", a);
    printf("b = 0x%02X (0b10101010)\n", b);
    printf("a & b  (AND):  0x%02X\n", a & b);
    printf("a | b  (OR):   0x%02X\n", a | b);
    printf("a ^ b  (XOR):  0x%02X\n", a ^ b);
    printf("~a     (NOT):  0x%08X\n", ~a);
    printf("a << 2 (LSH):  0x%02X\n", a << 2);
    printf("a >> 2 (RSH):  0x%02X\n", a >> 2);
}

void demo_assignment_operators(void) {
    DEMO_SECTION("Assignment Operators");
    int x = 10;
    printf("x = %d\n", x);
    x += 5; printf("x += 5:  %d\n", x);
    x -= 3; printf("x -= 3:  %d\n", x);
    x *= 2; printf("x *= 2:  %d\n", x);
    x /= 4; printf("x /= 4:  %d\n", x);
    x %= 3; printf("x %%= 3: %d\n", x);
    x = 0b1111;
    x &= 0b1010; printf("x &= 0b1010: 0x%X\n", x);
    x |= 0b0101; printf("x |= 0b0101: 0x%X\n", x);
    x ^= 0b1111; printf("x ^= 0b1111: 0x%X\n", x);
}

void demo_misc_operators(void) {
    DEMO_SECTION("Miscellaneous Operators");
    int a = 5, b = 10;
    printf("Ternary: (a > b) ? a : b = %d\n", (a > b) ? a : b);
    printf("sizeof(int) = %zu\n", sizeof(int));
    printf("Comma: (a=1, b=2, a+b) = %d\n", (a=1, b=2, a+b));
    int arr[5] = {10,20,30,40,50};
    int *p = arr;
    printf("Pointer deref: *p = %d\n", *p);
    printf("Address of: &a = %p\n", (void*)&a);
    printf("Array subscript: arr[2] = %d\n", arr[2]);
    struct { int x; } s = {42};
    struct { int x; } *sp = &s;
    printf("Member access: s.x = %d\n", s.x);
    printf("Ptr member: sp->x = %d\n", sp->x);
}

void demo_operator_precedence(void) {
    DEMO_SECTION("Operator Precedence");
    int a = 2, b = 3, c = 4;
    printf("a + b * c = %d (mul before add)\n", a + b * c);
    printf("(a + b) * c = %d (parens first)\n", (a + b) * c);
    printf("a || b && c = %d (&& before ||)\n", a || b && c);
    printf("Precedence (high to low):\n");
    printf("  () [] -> .  |  ! ~ ++ -- + - * & sizeof  |  * / %%\n");
    printf("  + -  |  << >>  |  < <= > >=  |  == !=  |  &  |  ^\n");
    printf("  |  |  &&  |  ||  |  ?:  |  = += -= etc  |  ,\n");
}

int main(void) {
    printf("\n=== C OPERATORS DEMO v%s ===\n", VERSION_STRING);
    demo_arithmetic_operators();
    demo_relational_operators();
    demo_logical_operators();
    demo_bitwise_operators();
    demo_assignment_operators();
    demo_misc_operators();
    demo_operator_precedence();
    DEMO_END(); return 0;
}
