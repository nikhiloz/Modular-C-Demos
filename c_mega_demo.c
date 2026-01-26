#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==== Structures, Enums, Macros ====
#define PI 3.14159
#define SQUARE(x) ((x)*(x))

enum Weekday { MON, TUE, WED, THU, FRI, SAT, SUN };

struct Student {
    char name[20];
    int age;
};

// ==== Function prototypes ====
void demo_arrays();
void demo_strings();
void demo_pointers();
void demo_structs();
void demo_fileio();
void demo_dynamic_memory();
void demo_recursion();
void demo_bitwise();
void demo_linkedlist();
void demo_enums_macros();
void demo_commandline(int argc, char *argv[]);
void demo_error_handling();

// ==== Linked List ====
struct Node {
    int data;
    struct Node *next;
};
void demo_linkedlist() {
    struct Node *head = malloc(sizeof(struct Node));
    head->data = 1;
    head->next = malloc(sizeof(struct Node));
    head->next->data = 2;
    head->next->next = NULL;

    printf("Linked list: %d -> %d\n", head->data, head->next->data);

    free(head->next);
    free(head);
}

// ==== Main Menu ====
int main(int argc, char *argv[]) {
    int choice;
    while (1) {
        printf("\n=== Mega C Feature Demo Menu ===\n");
        printf("1. Arrays\n");
        printf("2. Strings\n");
        printf("3. Pointers\n");
        printf("4. Structures\n");
        printf("5. File I/O\n");
        printf("6. Dynamic Memory\n");
        printf("7. Recursion\n");
        printf("8. Bitwise Operations\n");
        printf("9. Linked List\n");
        printf("10. Enums & Macros\n");
        printf("11. Command-line Arguments\n");
        printf("12. Error Handling\n");
        printf("13. Exit\n");
        printf("Enter choice: ");
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
            case 9: demo_linkedlist(); break;
            case 10: demo_enums_macros(); break;
            case 11: demo_commandline(argc, argv); break;
            case 12: demo_error_handling(); break;
            case 13: exit(0);
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
        perror("File open failed");
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

void demo_enums_macros() {
    enum Weekday today = WED;
    printf("Today enum value: %d\n", today);
    printf("Square of 4 using macro: %d\n", SQUARE(4));
    printf("PI constant: %.2f\n", PI);
}

void demo_commandline(int argc, char *argv[]) {
    printf("Program called with %d arguments:\n", argc);
    for (int i=0; i<argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

void demo_error_handling() {
    FILE *fp = fopen("nonexistent.txt", "r");
    if (!fp) {
        perror("Error opening file");
    } else {
        fclose(fp);
    }
}