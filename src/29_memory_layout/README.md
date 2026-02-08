# Chapter 29 — Process Memory Layout

## Overview
Every running C program occupies a precisely structured virtual address space — from the read-only `.text` segment at the bottom through the heap growing upward and the stack growing downward. This chapter maps out every region of a process's memory, demonstrates live address inspection, and dives into heap internals (brk vs mmap thresholds) and ASLR. The included demos print actual addresses and parse `/proc/self/maps` so you can see the layout firsthand.

## Key Concepts
- The six canonical segments: `.text`, `.rodata`, `.data`, `.bss`, heap, stack
- Memory-mapped region (`mmap`) between heap and stack
- Live address printing: taking addresses of globals, locals, and heap allocations
- Heap internals: `brk()` for small allocations vs `mmap()` for large ones (typically >128 KB)
- Stack growth direction and default stack size (`ulimit -s`)
- `/proc/self/maps` — the kernel's view of your address space
- ASLR: how the kernel randomises segment base addresses
- Thread stacks and their placement in the mmap region

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | Segment Overview | .text, .rodata, .data, .bss — what goes where and why |
| 2 | Heap Region | `brk()` vs `mmap()` — how malloc manages heap memory |
| 3 | Stack Region | Stack growth, frame layout, and `ulimit -s` |
| 4 | mmap Region | Shared libraries, large allocations, and anonymous mappings |
| 5 | Live Address Demo | Printing addresses of code, globals, locals, and heap pointers |
| 6 | Reading /proc/self/maps | Parsing the kernel's memory map from within the program |
| 7 | ASLR in Practice | Running the demo twice and comparing addresses |

## Building & Running
```bash
make bin/29_memory_layout
./bin/29_memory_layout
```

## Diagrams
- [Concept Diagram](29_memory_layout_concept.png)
- [Code Flow Diagram](29_memory_layout_flow.png)

## Try It Yourself
```bash
# View your shell's own memory map
cat /proc/self/maps

# Show a compact memory map of the demo
pmap $(pidof 29_memory_layout)

# Check process memory stats
cat /proc/self/status | grep -i vm

# Run twice to observe ASLR randomisation
./bin/29_memory_layout | grep "stack"
./bin/29_memory_layout | grep "stack"

# Disable ASLR temporarily and compare
setarch $(uname -m) -R ./bin/29_memory_layout | grep "stack"
setarch $(uname -m) -R ./bin/29_memory_layout | grep "stack"

# Check the current ASLR setting
cat /proc/sys/kernel/randomize_va_space
```

## Further Reading
- [man 5 proc](https://man7.org/linux/man-pages/man5/proc.5.html) — `/proc/[pid]/maps` documentation
- Gustavo Duarte, [Anatomy of a Program in Memory](https://manybutfinite.com/post/anatomy-of-a-program-in-memory/)
- [man 2 mmap](https://man7.org/linux/man-pages/man2/mmap.2.html) — memory mapping system call
