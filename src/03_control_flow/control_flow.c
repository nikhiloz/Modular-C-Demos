/**
 * @file control_flow.c
 * @brief C control flow: if/else, switch, for, while, do-while, goto
 */
#include "../../include/common.h"

void demo_if_else(void) {
    DEMO_SECTION("If-Else Statements");
    int x = 15;
    if (x > 20) printf("x > 20\n");
    else if (x > 10) printf("10 < x <= 20 (x=%d)\n", x);
    else printf("x <= 10\n");
    int a = 5, b = 10;
    int max = (a > b) ? a : b;
    printf("Ternary: max(%d,%d) = %d\n", a, b, max);
}

void demo_switch(void) {
    DEMO_SECTION("Switch Statement");
    int day = 3;
    switch (day) {
        case 1: printf("Monday\n"); break;
        case 2: printf("Tuesday\n"); break;
        case 3: printf("Wednesday\n"); break;
        case 6: case 7: printf("Weekend\n"); break;
        default: printf("Other day\n");
    }
}

void demo_for_loop(void) {
    DEMO_SECTION("For Loop");
    printf("Basic: "); for (int i = 0; i < 5; i++) printf("%d ", i); printf("\n");
    printf("Step 2: "); for (int i = 0; i < 10; i += 2) printf("%d ", i); printf("\n");
    printf("Nested 3x3:\n");
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) printf("%2d ", i*j);
        printf("\n");
    }
}

void demo_while_loop(void) {
    DEMO_SECTION("While Loop");
    int n = 5;
    printf("Countdown: "); while (n > 0) printf("%d ", n--); printf("Liftoff!\n");
}

void demo_do_while(void) {
    DEMO_SECTION("Do-While Loop");
    int n = 0;
    printf("Do-while: "); do { printf("%d ", n++); } while (n < 3); printf("\n");
    n = 10;
    printf("False cond: "); do { printf("%d ", n); } while (n < 5);
    printf("(runs once)\n");
}

void demo_break_continue(void) {
    DEMO_SECTION("Break and Continue");
    printf("Continue (skip even): ");
    for (int i = 0; i < 8; i++) { if (i % 2 == 0) continue; printf("%d ", i); }
    printf("\n");
    printf("Break at 5: ");
    for (int i = 0; i < 10; i++) { if (i == 5) break; printf("%d ", i); }
    printf("\n");
}

void demo_goto(void) {
    DEMO_SECTION("Goto (error handling pattern)");
    int *p = malloc(sizeof(int));
    if (!p) goto cleanup;
    *p = 42;
    printf("Allocated *p = %d\n", *p);
cleanup:
    free(p);
    printf("Cleanup done\n");
}

int main(void) {
    printf("\n=== C CONTROL FLOW DEMO v%s ===\n", VERSION_STRING);
    demo_if_else(); demo_switch(); demo_for_loop();
    demo_while_loop(); demo_do_while();
    demo_break_continue(); demo_goto();
    DEMO_END(); return 0;
}
