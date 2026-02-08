# Chapter 35 — Cross-Compilation

## Overview

Cross-compilation is the process of building executable code on one machine (the **build host**) that is intended to run on a different machine (the **target**) — typically with a different CPU architecture, operating system, or both. This is essential in embedded development where the target device (an ARM SBC, a MIPS router, an ESP32) lacks the resources to compile its own software. This chapter covers the terminology, toolchain anatomy, Makefile and CMake integration patterns, and the major build-system frameworks (Buildroot, Yocto, ESP-IDF) that automate cross-compilation at scale.

## Key Concepts

- Build / host / target terminology: the three machines in the GNU configure model
- Toolchain triplets: `arch-vendor-os-abi` (e.g., `aarch64-linux-gnu`, `arm-none-eabi`)
- Sysroot: the target's root filesystem used to resolve headers and libraries during cross-builds
- Endianness detection: compile-time checks and runtime verification across architectures
- Cross-compilation workflow: configure → build → deploy → test cycle
- Makefile `CROSS_COMPILE` pattern: prefixing `CC`, `LD`, `AR`, `STRIP` with the toolchain triplet
- CMake toolchain files: `CMAKE_SYSTEM_NAME`, `CMAKE_C_COMPILER`, `CMAKE_FIND_ROOT_PATH`
- Common pitfalls: host-path leakage, missing target libraries, endianness mismatches, float ABI confusion
- Build-system frameworks: Buildroot (simple, Kconfig-driven), Yocto (layer-based, recipe-driven), ESP-IDF (CMake + Kconfig for ESP32)

## Sections

| # | Section | Description |
|---|---------|-------------|
| 1 | Terminology | Build, host, and target machines; native vs cross compilation |
| 2 | Toolchain Anatomy | Triplets, binutils, cross-GCC, C library (glibc / musl / newlib) |
| 3 | Sysroot | What it contains, `--sysroot` flag, staged installs |
| 4 | Endianness | Big-endian vs little-endian, detection macros, byte-swap utilities |
| 5 | Workflow | Configure → cross-build → deploy (scp/TFTP) → run/debug on target |
| 6 | Makefile Pattern | `CROSS_COMPILE` variable, conditional tool prefixing |
| 7 | CMake Toolchain Files | Writing and using a `.cmake` toolchain file for cross-builds |
| 8 | Common Pitfalls | Host contamination, ABI mismatches, missing pkg-config wrappers |
| 9 | Build Systems | Buildroot, Yocto/OE, and ESP-IDF cross-compilation ecosystems |

## Building & Running

```bash
make bin/35_cross_compilation
./bin/35_cross_compilation
```

## Diagrams

- [Concept Diagram](35_cross_compilation_concept.png)
- [Code Flow Diagram](35_cross_compilation_flow.png)

## Try It Yourself

```bash
# --- Check your native binary ---
file ./bin/35_cross_compilation
# ELF 64-bit LSB executable, x86-64 ...

# --- Cross-compile for AArch64 (requires aarch64-linux-gnu toolchain) ---
aarch64-linux-gnu-gcc -o hello_arm64 hello.c
file hello_arm64
# ELF 64-bit LSB executable, ARM aarch64 ...

# --- Run the cross-compiled binary with QEMU user-mode ---
qemu-aarch64-static -L /usr/aarch64-linux-gnu ./hello_arm64

# --- Makefile CROSS_COMPILE pattern ---
make CROSS_COMPILE=aarch64-linux-gnu- bin/35_cross_compilation

# --- CMake with a toolchain file ---
cmake -B build-arm64 \
      -DCMAKE_TOOLCHAIN_FILE=toolchain-aarch64.cmake \
      -DCMAKE_BUILD_TYPE=Release
cmake --build build-arm64

# --- Install a cross-toolchain (Debian/Ubuntu) ---
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
                 qemu-user-static
```

## Further Reading

- [Mastering Embedded Linux Programming — Chris Simmonds](https://www.packtpub.com/product/mastering-embedded-linux-programming/9781789530384) — Chapter 2: Learning About Toolchains
- [CMake Cross Compiling Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [Buildroot User Manual](https://buildroot.org/downloads/manual/manual.html)
