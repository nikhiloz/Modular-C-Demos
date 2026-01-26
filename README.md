# Comprehensive C Programming Demos

A complete, modular demonstration of the C programming language covering all fundamental and advanced concepts.

## Project Structure

```
generic/
├── include/
│   └── common.h          # Shared macros, types, and utilities
├── src/
│   ├── main.c            # Master demo runner
│   ├── 01_basics/        # Data types and fundamentals
│   ├── 02_operators/     # All C operators
│   ├── 03_control_flow/  # Conditionals and loops
│   ├── 04_functions/     # Function concepts
│   ├── 05_arrays/        # Array handling
│   ├── 06_pointers/      # Pointer operations
│   ├── 07_strings/       # String manipulation
│   ├── 08_structures/    # Struct, union, enum
│   ├── 09_memory/        # Dynamic memory
│   ├── 10_file_io/       # File operations
│   ├── 11_preprocessor/  # Macros and conditionals
│   ├── 12_bitwise/       # Bit operations
│   ├── 13_advanced/      # C11/C99 features
│   ├── 14_concurrency/   # POSIX threads
│   └── 15_system/        # System programming
├── tests/                # Unit tests
├── bin/                  # Compiled binaries (generated)
├── Makefile
└── README.md
```

## Topics Covered

### 1. Data Types (01_basics)
- Integer types (char, short, int, long, long long)
- Floating point (float, double, long double)
- Fixed-width types (int8_t, uint32_t, etc.)
- Type qualifiers (const, volatile, restrict)
- Storage classes (auto, static, extern, register)

### 2. Operators (02_operators)
- Arithmetic (+, -, *, /, %)
- Relational (==, !=, <, >, <=, >=)
- Logical (&&, ||, !)
- Bitwise (&, |, ^, ~, <<, >>)
- Assignment (=, +=, -=, etc.)
- Ternary operator (?:)
- sizeof, comma operators

### 3. Control Flow (03_control_flow)
- if-else statements
- switch-case
- for, while, do-while loops
- break, continue
- goto and labels
- Nested control structures

### 4. Functions (04_functions)
- Function declaration/definition
- Pass by value vs reference
- Recursion (factorial, Fibonacci)
- Variadic functions (like printf)
- Function pointers
- Callbacks
- Inline functions

### 5. Arrays (05_arrays)
- One-dimensional arrays
- Multi-dimensional arrays (2D, 3D)
- Variable Length Arrays (VLA)
- Array-pointer relationship
- Passing arrays to functions
- Character arrays

### 6. Pointers (06_pointers)
- Pointer basics and arithmetic
- Double pointers (pointer to pointer)
- Array of pointers
- Pointer to array
- const with pointers
- void pointers
- Dynamic arrays

### 7. Strings (07_strings)
- String fundamentals
- string.h functions (strlen, strcpy, strcmp, etc.)
- String searching (strchr, strstr)
- Tokenization (strtok)
- Conversion (atoi, strtol, sprintf)
- ctype.h functions
- Memory functions (memcpy, memset)

### 8. Structures (08_structures)
- Basic struct definition
- typedef with structs
- Nested structures
- Self-referential (linked list)
- Unions
- Bit fields
- Enumerations
- Structure alignment

### 9. Memory Management (09_memory)
- malloc(), calloc(), realloc()
- free() and memory leaks
- 2D dynamic arrays
- Memory alignment
- Common patterns

### 10. File I/O (10_file_io)
- Text file operations
- Binary file operations
- File positioning (fseek, ftell)
- Error handling
- Buffered vs unbuffered I/O

### 11. Preprocessor (11_preprocessor)
- Object-like macros
- Function-like macros
- Stringification and concatenation
- Predefined macros (__FILE__, __LINE__, etc.)
- Conditional compilation
- Include guards

### 12. Bitwise Operations (12_bitwise)
- AND, OR, XOR, NOT
- Left/right shifts
- Bit manipulation patterns
- Bit masks
- Common bit tricks

### 13. Advanced Features (13_advanced)
- Compound literals
- Designated initializers
- _Generic (type-generic macros)
- Flexible array members
- _Alignas and _Alignof
- _Static_assert
- restrict pointer qualifier

### 14. Concurrency (14_concurrency)
- POSIX threads (pthread)
- Thread creation and joining
- Mutex synchronization
- Condition variables
- Producer-consumer pattern
- Thread-local storage

### 15. System Programming (15_system)
- Environment variables
- Signal handling
- Process information
- fork() and exec()
- system() calls
- Time functions

## Building

```bash
# Build all demos
make

# Build and run all
make test

# Run specific demo
make run-01_data_types
make run-06_pointers

# Clean build artifacts
make clean

# Show help
make help
```

## Requirements

- GCC or Clang compiler
- POSIX-compliant system (Linux, macOS, Android/Termux)
- pthread library
- C99/C11 support

## Usage Examples

```bash
# Run individual demo
./bin/01_data_types

# Run all demos
make test

# Compile single file manually
gcc -Wall -I include src/01_basics/data_types.c -o test_types
```

## C Standards Used

- **C89/C90**: Basic features, widely compatible
- **C99**: VLA, inline, fixed-width types, designated initializers
- **C11**: _Generic, _Alignas, _Static_assert, threads

## Version

2.0.0 - Complete refactoring with modular structure

## License

MIT License - Educational use
