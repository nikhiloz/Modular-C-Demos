#include "../../include/common.h"
#include <ctype.h>

void demo(void) {
    DEMO_SECTION("Strings Demo");
    char str[] = "Hello World";
    printf("String: %s\n", str);
    printf("Length: %zu\n", strlen(str));
    
    char buf[50];
    strcpy(buf, str);
    strcat(buf, "!");
    printf("Concat: %s\n", buf);
    
    printf("Compare: %d\n", strcmp("abc", "abd"));
    DEMO_END();
}

int main(void) {
    printf("=== STRINGS DEMO ===\n");
    demo();
    return 0;
}