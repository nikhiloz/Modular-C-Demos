/**
 * @file file_io.c
 * @brief C file I/O: fopen, fread, fwrite, fseek
 */
#include "../../include/common.h"

void demo_text_io(void) {
    DEMO_SECTION("Text File I/O");
    FILE *f = fopen("/tmp/test.txt", "w");
    if (!f) { perror("fopen"); return; }
    fprintf(f, "Hello World\nNumber: %d\n", 42);
    fclose(f);
    printf("Wrote to /tmp/test.txt\n");
    
    f = fopen("/tmp/test.txt", "r");
    char line[100];
    printf("Read:\n");
    while (fgets(line, sizeof(line), f)) printf("  %s", line);
    fclose(f);
}

void demo_binary_io(void) {
    DEMO_SECTION("Binary File I/O");
    int data[] = {10, 20, 30, 40, 50};
    FILE *f = fopen("/tmp/test.bin", "wb");
    fwrite(data, sizeof(int), 5, f);
    fclose(f);
    
    int buf[5];
    f = fopen("/tmp/test.bin", "rb");
    fread(buf, sizeof(int), 5, f);
    fclose(f);
    printf("Binary read: ");
    for (int i = 0; i < 5; i++) printf("%d ", buf[i]);
    printf("\n");
}

void demo_fseek(void) {
    DEMO_SECTION("File Seeking");
    FILE *f = fopen("/tmp/test.txt", "r");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    printf("File size: %ld bytes\n", ftell(f));
    rewind(f);
    char c = fgetc(f);
    printf("First char: %c\n", c);
    fclose(f);
}

int main(void) {
    printf("\n=== C FILE I/O DEMO v%s ===\n", VERSION_STRING);
    demo_text_io(); demo_binary_io(); demo_fseek();
    DEMO_END(); return 0;
}
