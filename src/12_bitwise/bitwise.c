/*
 * Chapter 12 — Bitwise Operations
 *
 * Bits are the language hardware speaks.  Every register in every MCU,
 * every network protocol header, every permission flag is just bits.
 * Mastering bitwise ops is essential for embedded, systems, and crypto work.
 *
 * Topics covered:
 *   1. Basic bitwise operators: AND, OR, XOR, NOT
 *   2. Shift operators: left shift (multiply), right shift (divide)
 *   3. Signed vs unsigned shift behaviour
 *   4. Bit manipulation patterns: set, clear, toggle, check
 *   5. Classic bit tricks: power-of-2 test, popcount, XOR swap
 *   6. Bit masks: extracting and inserting fields
 *   7. Endianness: how byte order affects bit interpretation
 *   8. Hardware register simulation (embedded pattern)
 *
 * Build: make 12_bitwise
 * Run:   ./bin/12_bitwise
 *
 * Try these:
 *   printf '%08b\n' 0xCC               # bash binary display
 *   python3 -c "print(bin(0xCC))"      # quick binary check
 *   objdump -d bin/12_bitwise | grep shr  # see shift instructions
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════
 *  Helper: Print a value in binary
 * ════════════════════════════════════════════════════════════════ */

/* Print the low 'bits' bits of 'n' in MSB-first order */
static void print_binary(unsigned int n, int bits)
{
    for (int i = bits - 1; i >= 0; i--)
        printf("%d", (n >> i) & 1);
}

/* Print with label and hex */
static void show_bits(const char *label, unsigned int val, int bits)
{
    printf("  %-14s = 0x%02X = ", label, val & ((1u << bits) - 1));
    print_binary(val, bits);
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Basic Bitwise Operators
 * ════════════════════════════════════════════════════════════════ */

static void demo_basic_ops(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Basic Bitwise Operators                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    u8 a = 0xCC;   /* 1100 1100 */
    u8 b = 0xAA;   /* 1010 1010 */

    show_bits("a", a, 8);
    show_bits("b", b, 8);
    printf("\n");
    show_bits("a & b (AND)", a & b, 8);     /* bits set in BOTH       */
    show_bits("a | b (OR)",  a | b, 8);     /* bits set in EITHER     */
    show_bits("a ^ b (XOR)", a ^ b, 8);     /* bits that DIFFER       */
    show_bits("~a    (NOT)", (u8)~a, 8);    /* every bit flipped      */
    printf("\n");

    printf("  What happens:\n");
    printf("    AND — masks out bits (keep only where mask=1)\n");
    printf("    OR  — forces bits on (set where mask=1)\n");
    printf("    XOR — flips bits (toggle where mask=1)\n");
    printf("    NOT — inverts every bit\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Shift Operators
 * ════════════════════════════════════════════════════════════════ */

static void demo_shifts(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Shift Operators                         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    u8 x = 0x0F;   /* 0000 1111 = 15 */
    show_bits("x", x, 8);
    show_bits("x << 2", x << 2, 8);    /* multiply by 4 → 60 */
    show_bits("x >> 2", x >> 2, 8);    /* divide by 4   → 3  */
    printf("  Left shift by n  = multiply by 2^n\n");
    printf("  Right shift by n = divide by 2^n (unsigned)\n\n");

    /* Signed right shift: implementation-defined!
     * Most compilers do arithmetic shift (preserves sign bit)
     * but the C standard does NOT guarantee this. */
    i8 neg = -16;   /* 1111 0000 in two's complement */
    printf("  Signed right shift (implementation-defined!):\n");
    printf("    (i8)-16 >> 2 = %d", neg >> 2);
    if ((neg >> 2) == -4)
        printf("  (arithmetic shift — sign preserved)\n");
    else
        printf("  (logical shift — zero-filled)\n");
    printf("    Lesson: for portable code, shift UNSIGNED values only.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Bit Manipulation Patterns
 * ════════════════════════════════════════════════════════════════ */

static void demo_bit_manipulation(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Bit Manipulation Patterns               ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Using common.h macros: SET_BIT, CLEAR_BIT, TOGGLE_BIT, CHECK_BIT */
    u8 flags = 0x00;
    printf("  Using macros from common.h:\n");
    show_bits("initial", flags, 8);

    SET_BIT(flags, 3);                  /* flags |= (1 << 3) */
    show_bits("SET_BIT(3)", flags, 8);

    SET_BIT(flags, 7);
    show_bits("SET_BIT(7)", flags, 8);

    CLEAR_BIT(flags, 3);               /* flags &= ~(1 << 3) */
    show_bits("CLEAR_BIT(3)", flags, 8);

    TOGGLE_BIT(flags, 0);              /* flags ^= (1 << 0) */
    show_bits("TOGGLE_BIT(0)", flags, 8);

    printf("\n  CHECK_BIT(flags, 7) = %d  (is bit 7 set? yes)\n",
           CHECK_BIT(flags, 7));
    printf("  CHECK_BIT(flags, 3) = %d  (is bit 3 set? no)\n\n",
           CHECK_BIT(flags, 3));

    printf("  Pattern summary:\n");
    printf("    Set bit n:    x |=  (1 << n)\n");
    printf("    Clear bit n:  x &= ~(1 << n)\n");
    printf("    Toggle bit n: x ^=  (1 << n)\n");
    printf("    Check bit n:  (x >> n) & 1\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Classic Bit Tricks
 * ════════════════════════════════════════════════════════════════ */

static void demo_common_tricks(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Classic Bit Tricks                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Even/odd test: LSB determines parity */
    int n = 12;
    printf("  n=%d  →  n & 1 = %d  → %s\n", n, n & 1,
           (n & 1) == 0 ? "even" : "odd");

    /* Power-of-2 test: a power of 2 has exactly one bit set.
     * n & (n-1) clears the lowest set bit; if result is 0, only
     * one bit was set.  Must exclude n==0 (not a power of 2). */
    for (int v = 0; v <= 18; v++) {
        if (v != 0 && (v & (v - 1)) == 0)
            printf("  %2d is a power of 2\n", v);
    }
    printf("\n");

    /* XOR swap — a classic trick, but DON'T use it in real code.
     * It has no advantage over a temp variable, confuses the
     * optimizer, and fails if a and b are the same memory location. */
    int a = 5, b = 10;
    printf("  XOR swap (educational only — don't use in production):\n");
    printf("    Before: a=%d, b=%d\n", a, b);
    a ^= b; b ^= a; a ^= b;       /* works but is pointless */
    printf("    After:  a=%d, b=%d\n", a, b);
    printf("    Why avoid: no speed gain, breaks if &a == &b,\n");
    printf("    and the compiler optimises a temp swap to the same code.\n\n");

    /* Brian Kernighan's popcount: count set bits.
     * Trick: n & (n-1) clears the lowest set bit.
     * Loop runs exactly as many times as there are set bits. */
    u32 x = 0xB5;      /* 1011 0101 → 5 bits set */
    int count = 0;
    for (u32 t = x; t; t &= (t - 1))   /* clear lowest bit each pass */
        count++;
    printf("  Popcount of 0x%02X: %d set bits  (Kernighan's algorithm)\n", x, count);

    /* __builtin_popcount: GCC/Clang intrinsic, compiles to
     * a single POPCNT instruction on modern x86 */
    printf("  __builtin_popcount(0x%02X) = %d  (compiler intrinsic)\n\n",
           x, __builtin_popcount(x));
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Bit Masks — Extracting and Inserting Fields
 * ════════════════════════════════════════════════════════════════ */

static void demo_masks(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Bit Masks — Extracting Fields           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    u16 val = 0xABCD;
    printf("  Value: 0x%04X\n", val);
    printf("  Low byte:   0x%02X  (val & 0xFF)\n", val & 0xFF);
    printf("  High byte:  0x%02X  ((val >> 8) & 0xFF)\n", (val >> 8) & 0xFF);
    printf("  Bits [7:4]: 0x%X   ((val >> 4) & 0xF)\n\n", (val >> 4) & 0xF);

    /* Inserting a field: clear the target bits first, then OR in new value */
    u16 reg = 0xFF00;
    u16 new_low = 0x42;
    reg = (reg & 0xFF00) | (new_low & 0xFF);   /* insert into low byte */
    printf("  Insert 0x42 into low byte of 0xFF00 → 0x%04X\n\n", reg);

    printf("  What happens: masks AND out unwanted bits, then OR\n");
    printf("  merges fields together.  This is the fundamental pattern\n");
    printf("  for PACKing/UNPACKing protocol headers and registers.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Endianness
 * ════════════════════════════════════════════════════════════════ */

static void demo_endianness(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Endianness                              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Endianness = the order bytes are stored in memory.
     * It does NOT affect bit ORDER within a byte — only byte ORDER.
     * Bit operations work on values (registers), not memory layout,
     * so (x >> 8) always gives the "high byte" regardless of endianness. */

    u32 val = 0xDEADBEEF;
    u8 *bytes = (u8 *)&val;     /* peek at the raw memory layout */

    printf("  Value: 0x%08X\n", val);
    printf("  Bytes in memory order: ");
    for (int i = 0; i < 4; i++)
        printf("%02X ", bytes[i]);
    printf("\n");

    /* Determine endianness at runtime */
    u16 test = 0x0001;
    u8 *p = (u8 *)&test;
    if (p[0] == 0x01)
        printf("  This machine is LITTLE-endian (x86, ARM default)\n");
    else
        printf("  This machine is BIG-endian (network byte order)\n");
    printf("\n");

    printf("  Why it matters:\n");
    printf("    • Network protocols use big-endian (\"network byte order\")\n");
    printf("    • x86/ARM store little-endian\n");
    printf("    • Use htonl()/ntohl() when sending data over the network\n");
    printf("    • Bitwise ops (>>, <<, &, |) work on VALUES — they are\n");
    printf("      endian-independent.  Only raw memory access is affected.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Hardware Register Simulation
 * ════════════════════════════════════════════════════════════════ */

/* Simulate an embedded peripheral control register.
 * In real hardware, these would be volatile pointers to MMIO addresses.
 * This pattern is used in EVERY microcontroller datasheet. */

/* Register bit definitions (from a hypothetical UART peripheral) */
#define UART_CR_EN      BIT(0)      /* UART enable              */
#define UART_CR_TXEN    BIT(1)      /* Transmit enable          */
#define UART_CR_RXEN    BIT(2)      /* Receive enable           */
#define UART_CR_PARITY  BIT(3)      /* Parity enable            */
#define UART_CR_STOP2   BIT(4)      /* 2 stop bits (vs 1)       */
#define UART_CR_BAUD_MASK  (0x7 << 8)  /* bits [10:8] = baud rate  */
#define UART_CR_BAUD_SHIFT 8

static void demo_register_sim(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Hardware Register Simulation            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* In embedded code, this would be:
     * volatile u32 *UART_CR = (volatile u32 *)0x40001000; */
    u32 uart_cr = 0;       /* simulated control register */

    printf("  Simulating UART peripheral register configuration:\n\n");
    show_bits("initial", uart_cr, 16);

    /* Enable UART, TX, and RX */
    uart_cr |= UART_CR_EN | UART_CR_TXEN | UART_CR_RXEN;
    show_bits("enable TX+RX", uart_cr, 16);

    /* Set baud rate field (bits [10:8]) to 5 */
    uart_cr &= ~UART_CR_BAUD_MASK;                        /* clear field first */
    uart_cr |= (5u << UART_CR_BAUD_SHIFT) & UART_CR_BAUD_MASK;  /* set new value */
    show_bits("baud=5", uart_cr, 16);

    /* Disable parity without affecting other bits */
    uart_cr &= ~UART_CR_PARITY;
    show_bits("no parity", uart_cr, 16);

    printf("\n  What happens: every MCU peripheral (UART, SPI, I2C,\n");
    printf("  GPIO, timers) is controlled by memory-mapped registers.\n");
    printf("  You configure them by setting/clearing individual bits.\n");
    printf("  The BIT(), SET_BIT(), CLEAR_BIT() macros from common.h\n");
    printf("  are exactly what embedded drivers use.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n=== C BITWISE OPERATIONS DEMO v%s ===\n", VERSION_STRING);

    demo_basic_ops();
    demo_shifts();
    demo_bit_manipulation();
    demo_common_tricks();
    demo_masks();
    demo_endianness();
    demo_register_sim();

    DEMO_END();
    return 0;
}
