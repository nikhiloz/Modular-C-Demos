#include "../../include/common.h"

void demo(void) {
    DEMO_SECTION("Advanced C Features");
    
    // Compound literals
    int *p = (int[]){1, 2, 3};
    printf("Compound literal: %d, %d, %d\n", p[0], p[1], p[2]);
    
    // Designated initializers
    int arr[5] = {[2] = 20, [4] = 40};
    printf("Designated init: ");
    for(int i = 0; i < 5; i++) printf("%d ", arr[i]);
    printf("\n");
    
    DEMO_END();
}

int main(void) {
    printf("=== ADVANCED DEMO ===\n");
    demo();
    return 0;
}