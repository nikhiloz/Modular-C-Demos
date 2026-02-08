# Comprehensive C Programming Demos

A complete, modular demonstration of the C programming language — from fundamentals through compiler internals, program loading, and practical systems depth. 36 self-contained chapters, each with annotated C source code, PlantUML diagrams, and hands-on exercises.

## Project Structure

```
Modular-C-Demos/
├── include/
│   └── common.h              # Shared macros, types, and utilities
├── src/
│   ├── main.c                # Master demo runner
│   │
│   │── Part I: C Fundamentals
│   ├── 01_basics/            # Data types and fundamentals
│   ├── 02_operators/         # All C operators
│   ├── 03_control_flow/      # Conditionals and loops
│   ├── 04_functions/         # Function concepts
│   ├── 05_arrays/            # Array handling
│   ├── 06_pointers/          # Pointer operations
│   ├── 07_strings/           # String manipulation
│   ├── 08_structures/        # Struct, union, enum
│   ├── 09_memory/            # Dynamic memory
│   ├── 10_file_io/           # File operations
│   ├── 11_preprocessor/      # Macros and conditionals
│   ├── 12_bitwise/           # Bit operations
│   ├── 13_advanced/          # C11/C99 features
│   ├── 14_concurrency/       # POSIX threads
│   ├── 15_system/            # System programming
│   │
│   │── Part II: How the Compiler Works
│   ├── 16_compilation_overview/  # The 4-stage pipeline
│   ├── 17_preprocessor_deep/     # Preprocessor internals
│   ├── 18_lexical_analysis/      # Tokenisation & DFA
│   ├── 19_parsing_ast/           # Recursive descent & AST
│   ├── 20_semantic_analysis/     # Symbol tables & type checking
│   ├── 21_intermediate_repr/     # TAC, SSA, GIMPLE, LLVM IR
│   ├── 22_optimisation/          # Compiler optimisation passes
│   ├── 23_code_generation/       # Register alloc & x86-64 codegen
│   ├── 24_assembler_elf/         # Assembler & ELF object format
│   ├── 25_linker/                # Linking: static, dynamic, scripts
│   │
│   │── Part III: Program Loading & Execution
│   ├── 26_elf_executable/        # ELF executable format
│   ├── 27_kernel_exec/           # fork/exec & kernel-side loading
│   ├── 28_dynamic_linker/        # ld.so, PLT/GOT, dlopen
│   ├── 29_memory_layout/         # Process address space
│   ├── 30_crt_startup/           # _start, CRT files, atexit
│   ├── 31_calling_conventions/   # System V ABI, stack frames
│   ├── 32_termination/           # exit, signals, zombies
│   │
│   │── Part IV: Practical Depth
│   ├── 33_debugging_tools/       # GDB, Valgrind, sanitizers
│   ├── 34_libraries/             # Static & dynamic libraries
│   ├── 35_cross_compilation/     # Cross-compiling for ARM/RISC-V
│   └── 36_virtual_memory/        # Pages, TLB, mmap, COW
│
├── bin/                  # Compiled binaries (generated)
├── Makefile
└── README.md
```

Each chapter directory contains:
- **`*.c`** — Annotated demo source with ASCII-art diagrams and live demos
- **`README.md`** — Theory, key concepts, build instructions, try-it exercises
- **`*_concept.puml / .png`** — Concept diagram (architecture / data structures)
- **`*_flow.puml / .png`** — Code flow diagram (activity / sequence)

---

## Part I — C Fundamentals (Chapters 1–15)

| Ch | Topic | Key Concepts |
|----|-------|--------------|
| 01 | Data Types | int, float, fixed-width, qualifiers, storage classes |
| 02 | Operators | arithmetic, relational, logical, bitwise, ternary |
| 03 | Control Flow | if-else, switch, loops, break/continue, goto |
| 04 | Functions | pass-by-value/ref, recursion, variadic, function pointers |
| 05 | Arrays | 1D/2D/3D, VLA, array-pointer relationship |
| 06 | Pointers | arithmetic, double pointers, void*, const correctness |
| 07 | Strings | string.h, searching, tokenisation, conversions |
| 08 | Structures | struct, union, bit fields, enum, alignment |
| 09 | Memory | malloc/calloc/realloc/free, 2D dynamic arrays |
| 10 | File I/O | text, binary, seeking, buffered I/O |
| 11 | Preprocessor | macros, #/##, conditional compilation, include guards |
| 12 | Bitwise | AND/OR/XOR, shifts, masks, bit tricks |
| 13 | Advanced | compound literals, _Generic, flexible arrays, _Static_assert |
| 14 | Concurrency | pthreads, mutex, condition variables, producer-consumer |
| 15 | System | signals, fork/exec, environment, time functions |

## Part II — How the Compiler Works (Chapters 16–25)

| Ch | Topic | Key Concepts |
|----|-------|--------------|
| 16 | Compilation Pipeline | 4 stages: preprocess → compile → assemble → link |
| 17 | Preprocessor Deep | #include, macros, stringify/paste, conditional, pragma |
| 18 | Lexical Analysis | tokens, DFA, maximal munch, hand-written tokenizer |
| 19 | Parsing & AST | BNF grammar, recursive descent, AST construction |
| 20 | Semantic Analysis | symbol tables, scope stack, type checking, conversions |
| 21 | Intermediate Repr. | TAC, SSA + phi-nodes, GIMPLE, LLVM IR, GCC pipeline |
| 22 | Optimisation | constant folding, DCE, strength reduction, LICM, inlining |
| 23 | Code Generation | x86-64 registers, prologue/epilogue, graph colouring |
| 24 | Assembler & ELF | ELF sections, symbols, relocations, section flags |
| 25 | Linker | symbol resolution, relocation, static/dynamic, scripts |

## Part III — Program Loading & Execution (Chapters 26–32)

| Ch | Topic | Key Concepts |
|----|-------|--------------|
| 26 | ELF Executable | program headers, segments, INTERP, PIE/ASLR |
| 27 | Kernel Execution | fork/exec, execve kernel-side, initial stack, shebang |
| 28 | Dynamic Linker | ld.so, PLT/GOT lazy binding, PIC, dlopen/dlsym |
| 29 | Memory Layout | text/data/bss/heap/stack, /proc/self/maps, ASLR |
| 30 | CRT Startup | _start, __libc_start_main, .init_array, atexit |
| 31 | Calling Conventions | System V ABI, registers, red zone, variadic internals |
| 32 | Termination | exit/_exit/abort, signals, wait status, zombies, core dumps |

## Part IV — Practical Depth (Chapters 33–36)

| Ch | Topic | Key Concepts |
|----|-------|--------------|
| 33 | Debugging Tools | GDB, Valgrind, sanitizers, strace, binary analysis tools |
| 34 | Libraries | static (.a), shared (.so), visibility, pkg-config, soname |
| 35 | Cross-Compilation | toolchain triplets, sysroot, endianness, QEMU |
| 36 | Virtual Memory | pages, TLB, page faults, COW, mmap/mprotect, swap |

---

## Building

```bash
# Build everything (all 36 chapters)
make

# Build a specific part
make part1    # C Fundamentals (ch01-15)
make part2    # Compiler Internals (ch16-25)
make part3    # Program Loading (ch26-32)
make part4    # Practical Depth (ch33-36)

# Build and run all demos
make test

# Run a specific chapter
./bin/16_compilation_overview
./bin/28_dynamic_linker

# Clean build artefacts
make clean

# Show all targets
make help
```

## Requirements

- GCC compiler (tested with GCC 11+)
- POSIX-compliant system (Linux recommended for Parts II–IV)
- pthread library (Chapter 14)
- libdl (Chapter 28 — dlopen)
- PlantUML + Java (for diagram generation, optional)
- C99/C11 support

## Diagrams

Every chapter includes two PlantUML diagrams:

1. **Concept diagram** — architectural overview, data structures, or comparisons
2. **Code flow diagram** — activity/sequence diagram of the algorithm or process

Pre-generated PNGs are included. To regenerate:

```bash
# Single diagram
java -jar /usr/share/plantuml/plantuml.jar -tpng src/16_compilation_overview/compilation_overview_concept.puml

# All diagrams
find src -name "*.puml" -exec java -jar /usr/share/plantuml/plantuml.jar -tpng {} \;
```

## C Standards Used

- **C89/C90**: Basic features, widely compatible
- **C99**: VLA, inline, fixed-width types, designated initializers
- **C11**: _Generic, _Alignas, _Static_assert, threads

## Version

3.0.0 — Extended with compiler internals, program loading, and practical depth (Parts II–IV)

## License

MIT License — Educational use
