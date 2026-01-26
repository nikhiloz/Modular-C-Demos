#include "../../include/common.h"

void demo(void) {
    DEMO_SECTION("Arrays Demo");
    int arr[5] = {1, 2, 3, 4, 5};
    printf("Array: ");
    for(int i = 0; i < 5; i++) printf("%d ", arr[i]);
    printf("\n");
    DEMO_END();
}

int main(void) {
    printf("=== ARRAYS DEMO ===\n");
    demo();
    return 0;
}