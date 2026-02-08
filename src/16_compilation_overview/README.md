# Chapter 16 — The Compilation Pipeline

## Overview
Every C programmer runs `gcc main.c -o main`, but few pause to consider the four
distinct stages that transform human-readable source into an executable binary.
Understanding the compilation pipeline—preprocessing, compilation, assembly, and
linking—gives you the vocabulary to diagnose cryptic build errors, write better
Makefiles, and reason about what the toolchain is actually doing on your behalf.

## Key Concepts
- The four-stage pipeline: preprocess → compile → assemble → link
- `gcc -E` to stop after preprocessing
- `gcc -S` to stop after compilation (produces assembly)
- `gcc -c` to stop after assembly (produces object files)
- `-save-temps` to keep all intermediate files at once
- Object file format (ELF `.o`) and its sections
- The role of the linker in symbol resolution and relocation
- Translation units and separate compilation

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | Why Understand Compilation | Motivation: better debugging, build-system literacy, cross-compilation awareness |
| 2 | The Four Stages | Bird's-eye view of preprocess → compile → assemble → link |
| 3 | Preprocessing Stage | Macro expansion, header inclusion, conditional compilation (`gcc -E`) |
| 4 | Compilation Stage | Translation of C to target assembly, optimisation passes (`gcc -S`) |
| 5 | Assembly Stage | Conversion of assembly text to relocatable object code (`gcc -c`) |
| 6 | Linking Stage | Symbol resolution, relocation, final executable or shared library |
| 7 | See It In Action | End-to-end walkthrough using the demo source file |

## Building & Running
```bash
make bin/16_compilation_overview
./bin/16_compilation_overview
```

## Diagrams
- [Concept Diagram](16_compilation_overview_concept.png)
- [Code Flow Diagram](16_compilation_overview_flow.png)

## Try It Yourself
```bash
# Stop after preprocessing — view expanded macros and included headers
gcc -E src/16_compilation_overview/compilation_overview.c -o /tmp/stage1.i
head -60 /tmp/stage1.i

# Stop after compilation — view generated assembly
gcc -S src/16_compilation_overview/compilation_overview.c -o /tmp/stage2.s
cat /tmp/stage2.s

# Stop after assembly — produce a relocatable object file
gcc -c src/16_compilation_overview/compilation_overview.c -o /tmp/stage3.o
file /tmp/stage3.o

# Keep all intermediate files in one shot
gcc -save-temps src/16_compilation_overview/compilation_overview.c -o /tmp/demo
ls /tmp/compilation_overview.*

# Inspect the object file's disassembly
objdump -d /tmp/stage3.o | head -40
```

## Further Reading
- *Computer Systems: A Programmer's Perspective* (Bryant & O'Hallaron) — Chapter 7: Linking
- GCC Manual — [Overall Options](https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html)
- Ian Lance Taylor — [Linkers](https://lwn.net/Articles/276782/) (20-part LWN series)
