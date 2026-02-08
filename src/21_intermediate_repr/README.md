# Chapter 21 — Intermediate Representations

## Overview

Compilers rarely translate source code directly into machine instructions. Instead, they lower the program through one or more **Intermediate Representations (IRs)** — structured, language-independent formats that are easier to analyse, optimise, and transform. This chapter explores the most important IRs used in modern compilers, from Three-Address Code and SSA form to GCC's GIMPLE and LLVM IR, and traces the full GCC multi-level pipeline from GENERIC down to assembly.

## Key Concepts

- Three-Address Code (TAC) as a canonical low-level IR
- Static Single Assignment (SSA) form and its benefits for optimisation
- Phi (φ) nodes for merging values at control-flow join points
- GIMPLE — GCC's primary middle-end IR
- LLVM IR — a typed, SSA-based, portable IR
- GCC's multi-level IR pipeline: GENERIC → GIMPLE → GIMPLE-SSA → RTL → Assembly
- Lowering vs raising — moving between abstraction levels
- Why IRs decouple the front end from the back end

## Sections

| # | Section | Description |
|---|---------|-------------|
| 1 | Why IRs | Motivation for intermediate representations and decoupling front/back ends |
| 2 | Three-Address Code (TAC) | Flat instruction format: `x = y op z`, temporaries, labels |
| 3 | SSA Form | Each variable assigned exactly once; easier dataflow analysis |
| 4 | Phi Nodes | Merging values from different control-flow predecessors |
| 5 | GIMPLE | GCC's tree-based, C-like IR used for most middle-end passes |
| 6 | LLVM IR | Typed, SSA-based IR with explicit memory model and metadata |
| 7 | GCC IR Pipeline | Full journey: GENERIC → GIMPLE → GIMPLE-SSA → RTL → Assembly |
| 8 | IR Examples | Side-by-side comparison of the same function in TAC, GIMPLE, and LLVM IR |

## Building & Running

```bash
make bin/21_intermediate_repr
./bin/21_intermediate_repr
```

## Diagrams

- [Concept Diagram](21_intermediate_repr_concept.png)
- [Code Flow Diagram](21_intermediate_repr_flow.png)

## Try It Yourself

```bash
# Dump all GCC tree-level IR passes (creates many .tree files)
gcc -fdump-tree-all -c sample.c

# Dump only the GIMPLE representation
gcc -fdump-tree-gimple -c sample.c
cat sample.c.005t.gimple

# Dump all RTL-level IR passes
gcc -fdump-rtl-all -c sample.c

# Generate LLVM IR (requires clang)
clang -S -emit-llvm sample.c -o sample.ll
cat sample.ll
```

## Further Reading

- [GCC Internals — GIMPLE](https://gcc.gnu.org/onlinedocs/gccint/GIMPLE.html)
- [LLVM Language Reference Manual](https://llvm.org/docs/LangRef.html)
- Cooper & Torczon, *Engineering a Compiler*, 2nd ed. — Chapter 5: Intermediate Representations
