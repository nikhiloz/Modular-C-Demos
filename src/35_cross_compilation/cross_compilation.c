/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 35 — Cross-Compilation                                 ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: toolchains, triplets, sysroot, endianness, multiarch   ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What Is Cross-Compilation?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_what_is(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What Is Cross-Compilation?                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Cross-compilation: building code on one system (host)\n");
    printf("  to run on a DIFFERENT system (target).\n\n");

    printf("  Terminology:\n");
    printf("  ┌────────────┬──────────────────────────────────────┐\n");
    printf("  │ Term       │ Meaning                              │\n");
    printf("  ├────────────┼──────────────────────────────────────┤\n");
    printf("  │ Build      │ Machine that builds the compiler     │\n");
    printf("  │ Host       │ Machine that runs the compiler       │\n");
    printf("  │ Target     │ Machine that runs the output binary  │\n");
    printf("  └────────────┴──────────────────────────────────────┘\n\n");

    printf("  Common scenarios:\n");
    printf("    - x86-64 PC → ARM Raspberry Pi\n");
    printf("    - x86-64 PC → ARM embedded (STM32, ESP32)\n");
    printf("    - x86-64 Linux → x86-64 Windows (MinGW)\n");
    printf("    - x86-64 → RISC-V\n\n");

    printf("  Why cross-compile?\n");
    printf("    1. Target too slow for native compilation\n");
    printf("    2. Target has no OS (bare-metal / RTOS)\n");
    printf("    3. Target has limited storage / no toolchain\n");
    printf("    4. CI/CD builds for multiple architectures\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Toolchain Triplets
 * ════════════════════════════════════════════════════════════════════ */
static void demo_triplets(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Toolchain Triplets                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Format: <arch>-<vendor>-<os>[-<abi>]\n\n");

    printf("  Examples:\n");
    printf("  ┌──────────────────────────────┬────────────────────────┐\n");
    printf("  │ Triplet                      │ Target                 │\n");
    printf("  ├──────────────────────────────┼────────────────────────┤\n");
    printf("  │ x86_64-linux-gnu             │ x86-64 Linux (glibc)   │\n");
    printf("  │ aarch64-linux-gnu            │ ARM64 Linux            │\n");
    printf("  │ arm-linux-gnueabihf          │ ARM 32-bit hard-float  │\n");
    printf("  │ arm-none-eabi                │ ARM bare-metal         │\n");
    printf("  │ riscv64-linux-gnu            │ RISC-V 64 Linux        │\n");
    printf("  │ x86_64-w64-mingw32           │ Windows 64-bit         │\n");
    printf("  │ mips-linux-gnu               │ MIPS Linux             │\n");
    printf("  │ xtensa-esp32-elf             │ ESP32 (Xtensa)         │\n");
    printf("  └──────────────────────────────┴────────────────────────┘\n\n");

    printf("  A cross toolchain provides:\n");
    printf("    <triplet>-gcc          Cross-compiler\n");
    printf("    <triplet>-g++          C++ cross-compiler\n");
    printf("    <triplet>-as           Cross-assembler\n");
    printf("    <triplet>-ld           Cross-linker\n");
    printf("    <triplet>-ar           Cross-archiver\n");
    printf("    <triplet>-objdump      Cross-object dump\n");
    printf("    <triplet>-gdb          Cross-debugger\n\n");

    printf("  Install on Debian/Ubuntu:\n");
    printf("    sudo apt install gcc-aarch64-linux-gnu\n");
    printf("    sudo apt install gcc-arm-linux-gnueabihf\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Sysroot
 * ════════════════════════════════════════════════════════════════════ */
static void demo_sysroot(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Sysroot                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  The sysroot is a directory containing the target's:\n");
    printf("    /usr/include/   → Target system headers\n");
    printf("    /usr/lib/       → Target libraries (.a, .so)\n");
    printf("    /lib/           → Target runtime libraries\n\n");

    printf("  Usage:\n");
    printf("    aarch64-linux-gnu-gcc --sysroot=/path/to/sysroot main.c\n\n");

    printf("  How to get a sysroot:\n");
    printf("    1. Copy from target device:\n");
    printf("       rsync -a user@target:/usr/include /usr/lib ./sysroot/\n\n");
    printf("    2. Use a build system (Buildroot, Yocto):\n");
    printf("       Buildroot: output/host/<triplet>/sysroot/\n");
    printf("       Yocto: tmp/sysroots/<machine>/\n\n");
    printf("    3. Multiarch (Debian):\n");
    printf("       sudo dpkg --add-architecture arm64\n");
    printf("       sudo apt install libc6-dev:arm64\n\n");

    printf("  The --sysroot flag tells GCC to look in this directory\n");
    printf("  instead of /usr/include and /usr/lib for headers/libs.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Endianness
 * ════════════════════════════════════════════════════════════════════ */
static void demo_endianness(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Endianness                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Byte order for multi-byte values in memory:\n\n");

    printf("  Example: uint32_t value = 0x12345678\n");
    printf("  Address:      [0]  [1]  [2]  [3]\n");
    printf("  Little-endian: 78   56   34   12   (LSB first)\n");
    printf("  Big-endian:    12   34   56   78   (MSB first)\n\n");

    /* Detect endianness at runtime */
    uint32_t test = 0x01020304;
    uint8_t *p = (uint8_t *)&test;
    int is_little = (p[0] == 0x04);

    printf("  This system is: %s-endian\n\n",
           is_little ? "LITTLE" : "BIG");

    printf("  Architecture endianness:\n");
    printf("  ┌─────────────────┬──────────────┐\n");
    printf("  │ Architecture    │ Endianness   │\n");
    printf("  ├─────────────────┼──────────────┤\n");
    printf("  │ x86, x86-64    │ Little       │\n");
    printf("  │ ARM (default)  │ Little       │\n");
    printf("  │ ARM (BE mode)  │ Big          │\n");
    printf("  │ MIPS           │ Bi (config)  │\n");
    printf("  │ PowerPC        │ Big          │\n");
    printf("  │ RISC-V         │ Little       │\n");
    printf("  │ SPARC          │ Big          │\n");
    printf("  │ Network byte   │ Big          │\n");
    printf("  └─────────────────┴──────────────┘\n\n");

    printf("  Portable code: use htonl/ntohl for network data,\n");
    printf("  or explicit byte-swap macros for file formats.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Cross-Compilation Workflow
 * ════════════════════════════════════════════════════════════════════ */
static void demo_workflow(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Cross-Compilation Workflow                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Step-by-step for ARM64 Linux target:\n\n");

    printf("  1. Install toolchain:\n");
    printf("     sudo apt install gcc-aarch64-linux-gnu\n\n");

    printf("  2. Write portable code:\n");
    printf("     #include <stdint.h>   ← fixed-width types\n");
    printf("     #include <stddef.h>   ← size_t\n\n");

    printf("  3. Cross-compile:\n");
    printf("     aarch64-linux-gnu-gcc -o hello hello.c\n\n");

    printf("  4. Verify the binary:\n");
    printf("     file hello\n");
    printf("     → hello: ELF 64-bit LSB pie executable, ARM aarch64...\n\n");

    printf("  5. Transfer to target:\n");
    printf("     scp hello user@raspberry-pi:~/\n\n");

    printf("  6. Run on target:\n");
    printf("     ssh user@raspberry-pi './hello'\n\n");

    printf("  Alternative: test with QEMU user-mode emulation:\n");
    printf("     sudo apt install qemu-user-static\n");
    printf("     qemu-aarch64-static ./hello\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Makefile Cross-Compilation
 * ════════════════════════════════════════════════════════════════════ */
static void demo_makefile_cross(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Makefile for Cross-Compilation             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Pattern: override CC with CROSS_COMPILE prefix.\n\n");

    printf("  Makefile:\n");
    printf("    CROSS_COMPILE ?=          # default: native\n");
    printf("    CC = $(CROSS_COMPILE)gcc\n");
    printf("    AR = $(CROSS_COMPILE)ar\n");
    printf("    STRIP = $(CROSS_COMPILE)strip\n");
    printf("    CFLAGS = -Wall -Wextra -O2\n\n");
    printf("    app: main.c util.c\n");
    printf("        $(CC) $(CFLAGS) -o $@ $^\n\n");

    printf("  Native build:\n");
    printf("    make\n\n");

    printf("  Cross build for ARM64:\n");
    printf("    make CROSS_COMPILE=aarch64-linux-gnu-\n\n");

    printf("  CMake approach:\n");
    printf("    cmake -DCMAKE_TOOLCHAIN_FILE=arm64-toolchain.cmake ..\n\n");

    printf("  arm64-toolchain.cmake:\n");
    printf("    set(CMAKE_SYSTEM_NAME Linux)\n");
    printf("    set(CMAKE_SYSTEM_PROCESSOR aarch64)\n");
    printf("    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)\n");
    printf("    set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Common Pitfalls
 * ════════════════════════════════════════════════════════════════════ */
static void demo_pitfalls(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Common Pitfalls                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌────────────────────────┬────────────────────────────────┐\n");
    printf("  │ Pitfall                │ Solution                       │\n");
    printf("  ├────────────────────────┼────────────────────────────────┤\n");
    printf("  │ sizeof(long) differs   │ Use stdint.h fixed-width types │\n");
    printf("  │ sizeof(void*) differs  │ Use uintptr_t for ptr math     │\n");
    printf("  │ Unaligned access       │ Use memcpy for packed structs  │\n");
    printf("  │ Endianness mismatch    │ Use byte-swap for wire formats │\n");
    printf("  │ Host headers used      │ Use --sysroot correctly        │\n");
    printf("  │ Float support missing  │ Use -mfloat-abi=soft/hard      │\n");
    printf("  │ glibc vs musl          │ Match target C library         │\n");
    printf("  │ Linking host libs      │ Check with 'file' on .so files │\n");
    printf("  └────────────────────────┴────────────────────────────────┘\n\n");

    printf("  Portability checklist:\n");
    printf("    [x] Use stdint.h (int32_t not int)\n");
    printf("    [x] Use sizeof(), not magic numbers\n");
    printf("    [x] No inline assembly (or #ifdef per arch)\n");
    printf("    [x] No host-specific paths (/usr/lib/x86_64-*)\n");
    printf("    [x] Test with -Wconversion -Wsign-conversion\n");
    printf("    [x] Use 'file' to verify binary architecture\n");
    printf("    [x] Test with QEMU before deploying\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Build Systems for Embedded
 * ════════════════════════════════════════════════════════════════════ */
static void demo_build_systems(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Build Systems for Embedded Targets         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌──────────────┬──────────────────────────────────────────┐\n");
    printf("  │ System       │ Purpose                                  │\n");
    printf("  ├──────────────┼──────────────────────────────────────────┤\n");
    printf("  │ Buildroot    │ Generates complete root filesystem + SDK │\n");
    printf("  │ Yocto/OE     │ Enterprise-grade Linux BSP generation    │\n");
    printf("  │ crosstool-NG │ Build custom cross-toolchains            │\n");
    printf("  │ ESP-IDF      │ Espressif toolchain + build system       │\n");
    printf("  │ Zephyr       │ RTOS with built-in cross-build support   │\n");
    printf("  │ PlatformIO   │ Multi-platform embedded build tool       │\n");
    printf("  └──────────────┴──────────────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 35 — Cross-Compilation                             ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_what_is();
    demo_triplets();
    demo_sysroot();
    demo_endianness();
    demo_workflow();
    demo_makefile_cross();
    demo_pitfalls();
    demo_build_systems();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 35 — Cross-Compilation\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
