#include "../../include/common.h"

void demo(void) {
    DEMO_SECTION("Memory Demo");
    
    int *arr = malloc(5 * sizeof(int));
    for(int i = 0; i < 5; i++) arr[i] = i * 10;
    printf("Malloc: ");
    for(int i = 0; i < 5; i++) printf("%d ", arr[i]);
    printf("\n");
    free(arr);
    
    int *zeros = calloc(5, sizeof(int));
    printf("Calloc: ");
    for(int i = 0; i < 5; i++) printf("%d ", zeros[i]);
    printf("\n");
    free(zeros);
    
    DEMO_END();
}

int main(void) {
    printf("=== MEMORY DEMO ===\n");
    demo();
    return 0;
}