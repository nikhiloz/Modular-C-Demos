#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==== Function prototypes ====
void demo_arrays();
void demo_strings();
void demo_pointers();
void demo_structs();
void demo_fileio();
void demo_dynamic_memory();
void demo_recursion();
void demo_bitwise();

// ==== Main Menu ====
int main() {
    int choice;
    while (1) {
        printf("\n=== C Feature Demo Menu ===\n");
        printf("1. Arrays\n");
        printf("2. Strings\n");
        printf("3. Pointers\n");
        printf("4. Structures\n");
        printf("5. File I/O\n");
        printf("6. Dynamic Memory (malloc/free)\n");
        printf("7. Recursion (factorial)\n");
        printf("8. Bitwise Operations\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: demo_arrays(); break;
            case 2: demo_strings(); break;
            case 3: demo_pointers(); break;
            case 4: demo_structs(); break;
            case 5: demo_fileio(); break;
            case 6: demo_dynamic_memory(); break;
            case 7: demo_recursion(); break;
            case 8: demo_bitwise(); break;
            case 9: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}

// ==== Feature Implementations ====
void demo_arrays() {
    int arr[5] = {1,2,3,4,5};
    printf("Array elements: ");
    for (int i=0; i<5; i++) printf("%d ", arr[i]);
    printf("\n");
}

void demo_strings() {
    char str1[20] = "Hello";
    char str2[20] = "World";
    strcat(str1, str2);
    printf("Concatenated string: %s\n", str1);
}

void demo_pointers() {
    int x = 10;
    int *px = &x;
    printf("Value: %d, Address: %p\n", *px, (void*)px);
}

void demo_structs() {
    struct Student {
        char name[20];
        int age;
    };
    struct Student s = {"Alice", 21};
    printf("Student: %s, Age: %d\n", s.name, s.age);
}

void demo_fileio() {
    FILE *fp = fopen("demo.txt", "w");
    if (fp) {
        fprintf(fp, "This is a file I/O demo.\n");
        fclose(fp);
        printf("Wrote to demo.txt\n");
    } else {
        printf("File open failed!\n");
    }
}

void demo_dynamic_memory() {
    int *arr = malloc(5 * sizeof(int));
    for (int i=0; i<5; i++) arr[i] = i*i;
    printf("Dynamic array: ");
    for (int i=0; i<5; i++) printf("%d ", arr[i]);
    printf("\n");
    free(arr);
}

int factorial(int n) {
    if (n==0) return 1;
    return n * factorial(n-1);
}
void demo_recursion() {
    int n=5;
    printf("Factorial of %d = %d\n", n, factorial(n));
}

void demo_bitwise() {
    int a=6, b=3;
    printf("a & b = %d\n", a & b);
    printf("a | b = %d\n", a | b);
    printf("a ^ b = %d\n", a ^ b);
    printf("~a = %d\n", ~a);
}