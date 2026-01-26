/**
 * @file main.c
 * @brief Master demo runner - runs all C concept demos
 */
#include "../include/common.h"
#include <unistd.h>

void print_menu(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           COMPREHENSIVE C PROGRAMMING DEMOS                  ║\n");
    printf("║                    Version %s                              ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\nAvailable Demos:\n");
    printf("  1. Data Types      - int, float, char, fixed-width, qualifiers\n");
    printf("  2. Operators       - arithmetic, logical, bitwise, assignment\n");
    printf("  3. Control Flow    - if, switch, for, while, goto\n");
    printf("  4. Functions       - recursion, variadic, callbacks\n");
    printf("  5. Arrays          - 1D, 2D, VLA, pointers\n");
    printf("  6. Pointers        - basics, arithmetic, double pointers\n");
    printf("  7. Strings         - string.h, manipulation, tokenization\n");
    printf("  8. Structures      - struct, union, enum, bit fields\n");
    printf("  9. Memory          - malloc, calloc, realloc, patterns\n");
    printf(" 10. File I/O        - text, binary, seeking\n");
    printf(" 11. Preprocessor    - macros, conditionals, pragmas\n");
    printf(" 12. Bitwise         - AND, OR, XOR, shifts, tricks\n");
    printf(" 13. Advanced        - generics, compound literals, C11\n");
    printf(" 14. Concurrency     - pthreads, mutex, condition vars\n");
    printf(" 15. System          - signals, environment, fork\n");
    printf("  0. Exit\n");
    printf("\nEnter choice (1-15, 0 to exit): ");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("████████████████████████████████████████████████████████████████\n");
    printf("██                                                            ██\n");
    printf("██   COMPREHENSIVE C PROGRAMMING DEMONSTRATION SUITE          ██\n");
    printf("██   Covering all major aspects of the C language             ██\n");
    printf("██                                                            ██\n");
    printf("████████████████████████████████████████████████████████████████\n");
    printf("\nBuild: %s %s\n", BUILD_DATE, BUILD_TIME);
    printf("\nTo run individual demos, use:\n");
    printf("  ./bin/01_data_types\n");
    printf("  ./bin/02_operators\n");
    printf("  ./bin/03_control_flow\n");
    printf("  ... etc.\n");
    printf("\nOr run: make test  (to execute all demos)\n");
    printf("\nDemo Topics Covered:\n");
    printf("  - Basic: Data types, Operators, Control flow, Functions\n");
    printf("  - Intermediate: Arrays, Pointers, Strings, Structures\n");
    printf("  - Advanced: Memory, File I/O, Preprocessor, Bitwise\n");
    printf("  - Expert: C11 features, Concurrency, System programming\n");
    
    return 0;
}
