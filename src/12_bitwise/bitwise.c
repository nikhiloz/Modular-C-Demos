/**
 * @file bitwise.c
 * @brief C bitwise operations: AND, OR, XOR, NOT, shifts, bit manipulation
 */
#include "../../include/common.h"

void print_binary(unsigned int n, int bits) {
    for (int i = bits - 1; i >= 0; i--) printf("%d", (n >> i) & 1);
}

void demo_basic_ops(void) {
    DEMO_SECTION("Basic Bitwise Operations");
    unsigned int a = 0b11001100, b = 0b10101010;
    printf("a = 0x%02X = ", a); print_binary(a, 8); printf("\n");
    printf("b = 0x%02X = ", b); print_binary(b, 8); printf("\n");
    printf("a & b  = 0x%02X = ", a & b); print_binary(a & b, 8); printf(" (AND)\n");
    printf("a | b  = 0x%02X = ", a | b); print_binary(a | b, 8); printf(" (OR)\n");
    printf("a ^ b  = 0x%02X = ", a ^ b); print_binary(a ^ b, 8); printf(" (XOR)\n");
    printf("~a     = 0x%02X = ", (unsigned char)~a); print_binary((unsigned char)~a, 8); printf(" (NOT)\n");
}

void demo_shifts(void) {
    DEMO_SECTION("Bit Shifts");
    unsigned int x = 0b00001111;
    printf("x      = "); print_binary(x, 8); printf(" (%d)\n", x);
    printf("x << 2 = "); print_binary(x << 2, 8); printf(" (%d)\n", x << 2);
    printf("x >> 2 = "); print_binary(x >> 2, 8); printf(" (%d)\n", x >> 2);
    printf("Left shift = multiply by 2^n\n");
    printf("Right shift = divide by 2^n\n");
}

void demo_bit_manipulation(void) {
    DEMO_SECTION("Bit Manipulation Patterns");
    unsigned int flags = 0;
    printf("Initial: "); print_binary(flags, 8); printf("\n");
    
    /* Set bit */
    flags |= (1 << 3);
    printf("Set bit 3: "); print_binary(flags, 8); printf("\n");
    
    /* Clear bit */
    flags &= ~(1 << 3);
    printf("Clear bit 3: "); print_binary(flags, 8); printf("\n");
    
    /* Toggle bit */
    flags = 0b00001111;
    flags ^= (1 << 2);
    printf("Toggle bit 2: "); print_binary(flags, 8); printf("\n");
    
    /* Check bit */
    printf("Bit 0 set? %s\n", (flags & (1 << 0)) ? "yes" : "no");
    printf("Bit 4 set? %s\n", (flags & (1 << 4)) ? "yes" : "no");
}

void demo_common_tricks(void) {
    DEMO_SECTION("Common Bit Tricks");
    int n = 12;
    printf("n = %d\n", n);
    printf("Is even? %s (n & 1 == 0)\n", (n & 1) == 0 ? "yes" : "no");
    printf("Is power of 2? %s (n & (n-1) == 0)\n", (n & (n-1)) == 0 ? "yes" : "no");
    n = 16;
    printf("n = %d is power of 2? %s\n", n, (n & (n-1)) == 0 ? "yes" : "no");
    
    /* Swap without temp */
    int a = 5, b = 10;
    printf("Before XOR swap: a=%d, b=%d\n", a, b);
    a ^= b; b ^= a; a ^= b;
    printf("After XOR swap: a=%d, b=%d\n", a, b);
    
    /* Count set bits */
    unsigned int x = 0b10110101;
    int count = 0;
    for (unsigned int t = x; t; t >>= 1) count += t & 1;
    printf("Set bits in 0b10110101: %d\n", count);
}

void demo_masks(void) {
    DEMO_SECTION("Bit Masks");
    unsigned int val = 0xABCD;
    printf("Value: 0x%04X\n", val);
    printf("Low byte: 0x%02X (val & 0xFF)\n", val & 0xFF);
    printf("High byte: 0x%02X (val >> 8)\n", (val >> 8) & 0xFF);
    printf("Bits 4-7: 0x%X ((val >> 4) & 0xF)\n", (val >> 4) & 0xF);
}

int main(void) {
    printf("\n=== C BITWISE DEMO v%s ===\n", VERSION_STRING);
    demo_basic_ops();
    demo_shifts();
    demo_bit_manipulation();
    demo_common_tricks();
    demo_masks();
    DEMO_END(); return 0;
}
