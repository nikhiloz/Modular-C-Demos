# Chapter 20 — Semantic Analysis

## Overview
Semantic analysis is the compiler phase that answers the question: "Does this
syntactically valid program actually make sense?" It checks that variables are
declared before use, types are compatible across operations, and scope rules are
respected. This chapter implements a hash-map-based symbol table with
push/pop scope management, performs basic type checking with implicit-conversion
handling, and shows how GCC's warning flags surface the same kinds of issues in
real-world code.

## Key Concepts
- Symbol tables: storing name → type/attribute mappings
- Hash-map-based symbol table implementation
- Scope management with push (enter) and pop (leave) operations
- Type checking: ensuring operand/type compatibility
- Implicit conversions (integer promotion, usual arithmetic conversions)
- Type compatibility rules in C (`int` ↔ `long`, signed ↔ unsigned, etc.)
- GCC warning flags that perform semantic checks (`-Wall`, `-Wextra`, `-Wconversion`)
- The distinction between errors (fatal) and warnings (advisory)

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | What Is Semantic Analysis | Purpose and position in the compilation pipeline |
| 2 | Symbol Tables | Hash-map structure for tracking declarations |
| 3 | Scope Management | Nested scopes via a stack of symbol-table frames |
| 4 | Type Checking | Verifying that operations receive compatible types |
| 5 | Implicit Conversions | Integer promotions and the usual arithmetic conversions |
| 6 | Type Compatibility Rules | When C silently converts, warns, or rejects |
| 7 | GCC Warning Flags | Real-world flags that expose semantic issues |

## Building & Running
```bash
make bin/20_semantic_analysis
./bin/20_semantic_analysis
```

## Diagrams
- [Concept Diagram](20_semantic_analysis_concept.png)
- [Code Flow Diagram](20_semantic_analysis_flow.png)

## Try It Yourself
```bash
# Run the demo to see symbol-table and type-checking output
./bin/20_semantic_analysis

# Use GCC's semantic warnings on your own code
gcc -Wconversion -Wsign-conversion -Wshadow -o /dev/null src/20_semantic_analysis/semantic_analysis.c

# See every warning GCC can produce
gcc -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow \
    -o /dev/null src/20_semantic_analysis/semantic_analysis.c

# Compare: compile the same file with no warnings vs all warnings
gcc -w -o /dev/null src/20_semantic_analysis/semantic_analysis.c
gcc -Wall -Wextra -Wconversion -o /dev/null src/20_semantic_analysis/semantic_analysis.c

# Experiment: introduce a type mismatch or undeclared variable in the demo
# and observe the semantic-analysis output
```

## Further Reading
- *Compilers: Principles, Techniques, and Tools* (Aho, Lam, Sethi, Ullman) — Chapter 6: Intermediate-Code Generation (covers type checking)
- *Engineering a Compiler* (Cooper & Torczon) — Chapter 4: Context-Sensitive Analysis
- GCC Manual — [Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
