# Chapter 17 — The Preprocessor in Depth

## Overview
The C preprocessor is a powerful text-transformation engine that runs before any
real compilation begins. It handles `#include` file insertion, macro expansion,
conditional compilation, and more. Mastering it lets you write portable,
configurable code—but misusing it leads to subtle bugs that are notoriously hard
to track down. This chapter explores every major preprocessor feature with
concrete examples and common pitfalls.

## Key Concepts
- `#include` search paths and the difference between `" "` and `< >`
- Object-like vs function-like macros
- Stringification operator (`#`) and token-pasting operator (`##`)
- Variadic macros (`__VA_ARGS__`) and `__VA_OPT__`
- Conditional compilation (`#if`, `#ifdef`, `#ifndef`, `#elif`, `#else`, `#endif`)
- Predefined macros (`__FILE__`, `__LINE__`, `__DATE__`, `__STDC_VERSION__`, etc.)
- `#pragma` directives and compiler-specific extensions
- Include guards vs `#pragma once`

## Sections
| # | Section | Description |
|---|---------|-------------|
| 1 | How #include Works | Search paths, `" "` vs `< >`, recursive inclusion |
| 2 | Object-like and Function-like Macros | `#define` constants and parameterised macros |
| 3 | Stringification and Token Pasting | Using `#` to stringify and `##` to concatenate tokens |
| 4 | Variadic Macros | `__VA_ARGS__`, `__VA_OPT__`, and debug-print patterns |
| 5 | Conditional Compilation | `#if`, `#ifdef`, feature flags, platform guards |
| 6 | Predefined Macros | Standard and compiler-specific built-in macros |
| 7 | Pragma Directives | `#pragma once`, `#pragma pack`, `_Pragma` operator |
| 8 | Include Guards vs pragma once | Trade-offs, portability, and best practices |
| 9 | Pitfalls | Double-evaluation, macro hygiene, and debugging tips |

## Building & Running
```bash
make bin/17_preprocessor_deep
./bin/17_preprocessor_deep
```

## Diagrams
- [Concept Diagram](17_preprocessor_deep_concept.png)
- [Code Flow Diagram](17_preprocessor_deep_flow.png)

## Try It Yourself
```bash
# See the fully preprocessed output (all macros expanded, headers inlined)
gcc -E src/17_preprocessor_deep/preprocessor_deep.c | tail -80

# List every predefined macro for your compiler and target
gcc -dM -E - < /dev/null | sort

# Check which macros are defined for a C11 build
gcc -std=c11 -dM -E - < /dev/null | grep __STDC

# Preprocess with a custom feature flag
gcc -E -DFEATURE_LOGGING=1 src/17_preprocessor_deep/preprocessor_deep.c | grep -A5 "logging"
```

## Further Reading
- GCC Manual — [The C Preprocessor](https://gcc.gnu.org/onlinedocs/cpp/)
- *C Programming: A Modern Approach* (K. N. King) — Chapter 14: The Preprocessor
- cppreference — [Preprocessor](https://en.cppreference.com/w/c/preprocessor)
