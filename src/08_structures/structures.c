/*
 * Chapter 8 — Structures, Unions, Enums & Bit-Fields
 *
 * C's "build your own type" toolkit.  This chapter covers:
 *   1. Basic struct — declaration, initialization, member access
 *   2. Nested structs — composing types (Rectangle with Points)
 *   3. Pointers to structs — arrow operator, pass-by-pointer
 *   4. Unions — memory sharing, type punning, tagged unions
 *   5. Enums — named constants, explicit values, flags pattern
 *   6. Bit-fields — packing bits, hardware register simulation
 *   7. Alignment & padding — sizeof surprises, packed attribute
 *   8. Linked list — practical application of struct + pointers
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/08_structures \
 *            src/08_structures/structures.c
 * Run:   ./bin/08_structures
 *
 * Try these:
 *   - Reorder the fields in PaddedStruct — does sizeof change?
 *   - Add a SHAPE_TRIANGLE to the tagged union — what do you need?
 *   - Run with valgrind to verify the linked list has no leaks
 *   - Use pahole (if installed) to visualize struct layout
 */

#include "../../include/common.h"

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Basic Structs
 *  Declaration, initialization, member access, designated init.
 * ════════════════════════════════════════════════════════════════ */

typedef struct {
    char  name[32];
    int   age;
    float gpa;
} Student;

static void demo_basic_struct(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 1: Basic Structs                          ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Ordered initialization                                        */
    Student s1 = {"Alice", 20, 3.85f};
    printf("  Ordered init:     %s, age %d, GPA %.2f\n", s1.name, s1.age, s1.gpa);

    /* Designated initializers (C99) — order doesn't matter          */
    Student s2 = {.gpa = 3.92f, .name = "Bob", .age = 22};
    printf("  Designated init:  %s, age %d, GPA %.2f\n", s2.name, s2.age, s2.gpa);

    /* Partial initialization — unspecified members are zero         */
    Student s3 = {.name = "Charlie"};
    printf("  Partial init:     %s, age %d, GPA %.2f  (zeros!)\n\n", s3.name, s3.age, s3.gpa);

    printf("  sizeof(Student) = %zu bytes\n", sizeof(Student));
    printf("  (May be > sum of fields due to padding — see Section 7)\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Nested Structs
 *  Composing types — a Rectangle defined by two Points.
 * ════════════════════════════════════════════════════════════════ */

typedef struct {
    float x, y;
} Point;

typedef struct {
    Point top_left;
    Point bottom_right;
} Rectangle;

static void demo_nested(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 2: Nested Structs                         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    Rectangle r = {
        .top_left     = {1.0f, 5.0f},
        .bottom_right = {4.0f, 1.0f}
    };

    float width  = r.bottom_right.x - r.top_left.x;
    float height = r.top_left.y - r.bottom_right.y;

    printf("  Rectangle: (%.1f,%.1f) to (%.1f,%.1f)\n",
           r.top_left.x, r.top_left.y, r.bottom_right.x, r.bottom_right.y);
    printf("  Width  = %.1f\n", width);
    printf("  Height = %.1f\n", height);
    printf("  Area   = %.1f\n\n", width * height);

    printf("  Access pattern: outer.inner.field\n");
    printf("  sizeof(Rectangle) = %zu  (two Points = 2 × %zu)\n\n",
           sizeof(Rectangle), sizeof(Point));
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Pointers to Structs
 *  Arrow operator, pass-by-pointer vs pass-by-value.
 * ════════════════════════════════════════════════════════════════ */

static void birthday(Student *s)
{
    /* Arrow operator: s->age  is shorthand for  (*s).age            */
    s->age++;
}

static void demo_pointers_to_struct(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 3: Pointers to Structs                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    Student s = {"Diana", 25, 3.70f};
    Student *p = &s;

    /* Arrow vs dot                                                  */
    printf("  s.name  = \"%s\"     (dot: direct access)\n", s.name);
    printf("  p->name = \"%s\"     (arrow: through pointer)\n", p->name);
    printf("  (*p).name = \"%s\"   (equivalent, but ugly)\n\n", (*p).name);

    /* Pass by pointer — modifies original                           */
    printf("  Before birthday(): age = %d\n", s.age);
    birthday(&s);
    printf("  After  birthday(): age = %d  (modified in place!)\n\n", s.age);

    /* Pass by value copies the ENTIRE struct — expensive for large ones */
    printf("  sizeof(Student) = %zu bytes copied each call by value.\n", sizeof(Student));
    printf("  sizeof(Student*) = %zu bytes — always pass large structs by pointer.\n\n",
           sizeof(Student *));
}

/* ════════════════════════════════════════════════════════════════
 *  Section 4: Unions
 *  Memory sharing, type punning, tagged union pattern.
 * ════════════════════════════════════════════════════════════════ */

/* A tagged union: the 'type' field tells which member is valid      */
typedef enum { SHAPE_CIRCLE, SHAPE_RECT } ShapeType;

typedef struct {
    ShapeType type;
    union {
        struct { float radius; }                circle;
        struct { float width, height; }         rect;
    } data;
} Shape;

static float shape_area(const Shape *s)
{
    switch (s->type) {
        case SHAPE_CIRCLE: return 3.14159f * s->data.circle.radius * s->data.circle.radius;
        case SHAPE_RECT:   return s->data.rect.width * s->data.rect.height;
        default:           return 0.0f;
    }
}

static void demo_unions(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 4: Unions                                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Basic union: all members share the same memory                */
    union { int i; float f; char bytes[4]; } u;
    u.i = 0x41424344;
    printf("  union { int i; float f; char bytes[4]; };\n");
    printf("  u.i = 0x%X\n", u.i);
    printf("  u.bytes = ['%c','%c','%c','%c']  (same memory!)\n",
           u.bytes[0], u.bytes[1], u.bytes[2], u.bytes[3]);
    printf("  sizeof(union) = %zu  (size of largest member)\n\n", sizeof(u));

    /* Tagged union — safe, extensible pattern                       */
    Shape circle = { .type = SHAPE_CIRCLE, .data.circle = {5.0f} };
    Shape rect   = { .type = SHAPE_RECT,   .data.rect   = {3.0f, 4.0f} };

    printf("  Tagged union pattern (type + union):\n");
    printf("    Circle (r=5.0): area = %.2f\n", shape_area(&circle));
    printf("    Rect (3×4):     area = %.2f\n\n", shape_area(&rect));

    printf("  What happens: writing one union member and reading\n");
    printf("  another is 'type punning' — technically UB in C99,\n");
    printf("  but well-defined in many compilers (gcc, clang).\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 5: Enums
 *  Named constants, explicit values, bitmask/flags pattern.
 * ════════════════════════════════════════════════════════════════ */

static void demo_enums(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 5: Enums                                  ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* Basic enum: auto-numbered from 0                              */
    enum Color { RED, GREEN, BLUE };
    enum Color c = GREEN;
    printf("  enum Color { RED=0, GREEN=1, BLUE=2 };\n");
    printf("  c = GREEN → %d\n\n", c);

    /* Explicit values                                               */
    enum HttpStatus { OK = 200, NOT_FOUND = 404, SERVER_ERROR = 500 };
    printf("  enum HttpStatus { OK=200, NOT_FOUND=404, SERVER_ERROR=500 };\n");
    printf("  OK = %d, NOT_FOUND = %d\n\n", OK, NOT_FOUND);

    /* Flags pattern: each value is a power of 2 → combine with |   */
    enum Permissions {
        PERM_READ    = (1 << 0),   /* 0b0001 = 1 */
        PERM_WRITE   = (1 << 1),   /* 0b0010 = 2 */
        PERM_EXECUTE = (1 << 2)    /* 0b0100 = 4 */
    };

    unsigned int perms = PERM_READ | PERM_WRITE;
    printf("  Flags pattern: PERM_READ | PERM_WRITE = 0x%02X\n", perms);
    printf("  Has READ?    %s\n", (perms & PERM_READ)    ? "yes" : "no");
    printf("  Has EXECUTE? %s\n\n", (perms & PERM_EXECUTE) ? "yes" : "no");

    printf("  Note: C enums are just ints — no type safety.\n");
    printf("  You can assign any int to an enum variable.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 6: Bit-Fields
 *  Packing bits within a struct, hardware register simulation.
 * ════════════════════════════════════════════════════════════════ */

/* Simulating an 8-bit hardware status register                      */
typedef struct {
    unsigned int ready    : 1;  /* bit 0 */
    unsigned int error    : 1;  /* bit 1 */
    unsigned int mode     : 2;  /* bits 2-3 (4 possible values)      */
    unsigned int reserved : 4;  /* bits 4-7 */
} StatusReg;

static void demo_bit_fields(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 6: Bit-Fields                             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    StatusReg reg = {0};
    reg.ready = 1;
    reg.mode  = 3;             /* 2 bits → max value is 3            */

    printf("  StatusReg (simulated hardware register):\n");
    printf("    ready    (1 bit)  = %u\n", reg.ready);
    printf("    error    (1 bit)  = %u\n", reg.error);
    printf("    mode     (2 bits) = %u\n", reg.mode);
    printf("    reserved (4 bits) = %u\n", reg.reserved);
    printf("    sizeof(StatusReg) = %zu byte(s)\n\n", sizeof(StatusReg));

    printf("  Bit-fields are great for memory-mapped I/O registers.\n");
    printf("  Caveats: layout is compiler-dependent, not portable\n");
    printf("  across different architectures or compilers.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 7: Alignment & Padding
 *  Why sizeof(struct) > sum of fields, and packed structs.
 * ════════════════════════════════════════════════════════════════ */

/* This struct has padding between fields for alignment              */
struct Padded {
    char  a;    /*  1 byte  + 3 bytes padding                       */
    int   b;    /*  4 bytes (aligned to 4-byte boundary)            */
    char  c;    /*  1 byte  + 3 bytes padding (struct rounds up)    */
};

/* Reordered to minimize padding                                     */
struct Efficient {
    int   b;    /*  4 bytes                                         */
    char  a;    /*  1 byte                                          */
    char  c;    /*  1 byte  + 2 bytes padding (align to int)        */
};

/* Packed: no padding, but unaligned access may be slower            */
struct __attribute__((packed)) Packed {
    char  a;
    int   b;
    char  c;
};

static void demo_alignment(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 7: Alignment & Padding                    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("  struct Padded    { char a; int b; char c; };\n");
    printf("    sizeof = %zu  (expected 6, got %zu due to padding!)\n",
           sizeof(struct Padded), sizeof(struct Padded));

    printf("  struct Efficient { int b; char a; char c; };\n");
    printf("    sizeof = %zu   (reordered → less waste)\n", sizeof(struct Efficient));

    printf("  struct Packed    { char a; int b; char c; } __packed;\n");
    printf("    sizeof = %zu   (no padding, but unaligned access!)\n\n", sizeof(struct Packed));

    /* Show the offsets                                              */
    printf("  Padded field offsets:\n");
    printf("    a: offset %zu, b: offset %zu, c: offset %zu\n",
           offsetof(struct Padded, a), offsetof(struct Padded, b), offsetof(struct Padded, c));
    printf("  Efficient field offsets:\n");
    printf("    b: offset %zu, a: offset %zu, c: offset %zu\n\n",
           offsetof(struct Efficient, b), offsetof(struct Efficient, a), offsetof(struct Efficient, c));

    printf("  Rule: order fields largest-to-smallest to minimize padding.\n");
    printf("  Use __attribute__((packed)) only for wire protocols/hardware.\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Section 8: Linked List (Practical Application)
 *  Self-referential struct + dynamic allocation.
 * ════════════════════════════════════════════════════════════════ */

typedef struct Node {
    int data;
    struct Node *next;         /* self-referential: points to same type */
} Node;

/* Push a value onto the front of the list                           */
static Node *list_push(Node *head, int value)
{
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) { perror("malloc"); return head; }
    new_node->data = value;
    new_node->next = head;     /* new node points to old head        */
    return new_node;           /* new node becomes the head          */
}

/* Print and count all nodes                                         */
static void list_print(const Node *head)
{
    printf("  List: ");
    int count = 0;
    for (const Node *n = head; n; n = n->next) {
        printf("%d → ", n->data);
        count++;
    }
    printf("NULL  (%d nodes)\n", count);
}

/* Free all nodes — prevent memory leaks                             */
static void list_free(Node *head)
{
    while (head) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
}

static void demo_linked_list(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Section 8: Singly Linked List                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    Node *list = NULL;         /* empty list                         */
    list = list_push(list, 30);
    list = list_push(list, 20);
    list = list_push(list, 10);

    list_print(list);
    printf("  sizeof(Node) = %zu bytes  (int + pointer)\n\n", sizeof(Node));

    printf("  What happens: each node is malloc'd separately.\n");
    printf("  Traversal follows the ->next chain until NULL.\n");
    printf("  Must free every node or you leak memory.\n\n");

    list_free(list);           /* clean up all nodes                 */
}

/* ════════════════════════════════════════════════════════════════
 *  main — run all demos in order
 * ════════════════════════════════════════════════════════════════ */

int main(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 8 — Structures              v%s        ║\n", VERSION_STRING);
    printf("╚══════════════════════════════════════════════════════╝\n");

    demo_basic_struct();
    demo_nested();
    demo_pointers_to_struct();
    demo_unions();
    demo_enums();
    demo_bit_fields();
    demo_alignment();
    demo_linked_list();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: Structs are C's way of grouping related data.\n");
    printf(" Combine with pointers for dynamic data structures,\n");
    printf(" with unions for memory-efficient variants, and with\n");
    printf(" enums for readable named constants.\n");
    printf("════════════════════════════════════════════════════════\n");

    DEMO_END();
    return 0;
}
