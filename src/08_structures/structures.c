#include "../../include/common.h"

typedef struct { int x, y; } Point;
typedef struct { char name[50]; int age; } Person;

void demo(void) {
    DEMO_SECTION("Structures Demo");
    
    Point p = {10, 20};
    printf("Point: (%d, %d)\n", p.x, p.y);
    
    Person person = {"Alice", 30};
    printf("Person: %s, age %d\n", person.name, person.age);
    
    union { int i; float f; char c[4]; } u;
    u.i = 0x41424344;
    printf("Union int: 0x%X\n", u.i);
    
    enum Color { RED, GREEN, BLUE };
    enum Color c = GREEN;
    printf("Color: %d\n", c);
    
    DEMO_END();
}

int main(void) {
    printf("=== STRUCTURES DEMO ===\n");
    demo();
    return 0;
}