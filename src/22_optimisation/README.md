# Chapter 22 — Compiler Optimisations

## Overview

Once a program is lowered to an intermediate representation, the compiler applies a battery of **optimisation passes** to improve performance, reduce code size, or both — all while preserving the program's observable behaviour. This chapter surveys the most important optimisations from simple algebraic rewrites like constant folding to advanced loop transformations and interprocedural inlining, and shows how GCC's `-O` levels bundle them together.

## Key Concepts

- Optimisation levels: `-O0`, `-O1`, `-O2`, `-O3`, `-Os`, `-Ofast`
- Constant folding and constant propagation
- Dead code elimination (DCE)
- Strength reduction (replacing expensive ops with cheaper ones)
- Loop optimisations: LICM, unrolling, vectorisation
- Function inlining and its trade-offs
- Common Subexpression Elimination (CSE)
- Tail call optimisation (TCO)

## Sections

| # | Section | Description |
|---|---------|-------------|
| 1 | Why Optimise | Bridging the gap between readable source and fast machine code |
| 2 | Optimisation Levels | What `-O0` through `-Ofast` enable and their trade-offs |
| 3 | Constant Folding | Evaluating constant expressions at compile time |
| 4 | Dead Code Elimination | Removing code that cannot affect program output |
| 5 | Strength Reduction | Replacing multiply with shift, division with reciprocal multiply, etc. |
| 6 | Loop Optimisations | Loop-Invariant Code Motion, unrolling, and auto-vectorisation |
| 7 | Function Inlining | Replacing a call with the function body; when it helps and hurts |
| 8 | Common Subexpression Elimination | Reusing already-computed values instead of recomputing them |
| 9 | Tail Call Optimisation | Converting tail-recursive calls into jumps to avoid stack growth |

## Building & Running

```bash
make bin/22_optimisation
./bin/22_optimisation
```

## Diagrams

- [Concept Diagram](22_optimisation_concept.png)
- [Code Flow Diagram](22_optimisation_flow.png)

## Try It Yourself

```bash
# Compare unoptimised and optimised assembly side-by-side
gcc -S -O0 -o sample_O0.s sample.c
gcc -S -O2 -o sample_O2.s sample.c
diff sample_O0.s sample_O2.s

# View optimised assembly directly
gcc -S -O2 -masm=intel sample.c -o sample.s
cat sample.s

# See which optimisations the compiler applied
gcc -O2 -fopt-info-all -c sample.c

# List all passes enabled at -O2
gcc -O2 -Q --help=optimizers | grep enabled
```

## Further Reading

- [GCC Optimisation Options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- Aho, Lam, Sethi & Ullman, *Compilers: Principles, Techniques, and Tools*, 2nd ed. — Chapters 8–9
- Agner Fog, [Optimising Software in C++](https://www.agner.org/optimize/)
