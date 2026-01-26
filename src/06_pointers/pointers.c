#include "../../include/common.h"

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

void demo(void) {
    DEMO_SECTION("Pointers Demo");
    int x = 42;
    int *p = &x;
    printf("x = %d, *p = %d\n", x, *p);
    
    int (*op)(int, int) = add;
    printf("add(5,3) = %d\n", op(5, 3));
    op = sub;
    printf("sub(5,3) = %d\n", op(5, 3));
    DEMO_END();
}

int main(void) {
    printf("=== POINTERS DEMO ===\n");
    demo();
    return 0;
}