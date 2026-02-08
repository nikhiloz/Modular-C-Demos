/*
 * Chapter 20 — Semantic Analysis
 *
 * Demonstrates what happens after parsing — checking meaning:
 *   1. Symbol table: hash-map based, insert/lookup variables
 *   2. Scope: push/pop scope levels, inner shadows outer
 *   3. Type checking: detect type mismatches
 *   4. Implicit conversions: integer promotion, float↔int
 *   5. GCC warnings that correspond to semantic checks
 *
 * Build: gcc -Wall -Wextra -std=c99 -o bin/20_semantic_analysis \
 *            src/20_semantic_analysis/semantic_analysis.c
 * Run:   ./bin/20_semantic_analysis
 *
 * Try:
 *   gcc -Wall -Wshadow -Wconversion -Wuninitialized \
 *       src/20_semantic_analysis/semantic_analysis.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ════════════════════════════════════════════════════════════════
 *  Section 1: Type System — Definitions
 * ════════════════════════════════════════════════════════════════ */

/* Our mini type system */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_INT_PTR,       /* int*   */
    TYPE_FLOAT_PTR,     /* float* */
    TYPE_CHAR_PTR,      /* char*  */
    TYPE_VOID,
    TYPE_UNKNOWN
} VarType;

static const char *type_name(VarType t)
{
    switch (t) {
        case TYPE_INT:       return "int";
        case TYPE_FLOAT:     return "float";
        case TYPE_CHAR:      return "char";
        case TYPE_INT_PTR:   return "int*";
        case TYPE_FLOAT_PTR: return "float*";
        case TYPE_CHAR_PTR:  return "char*";
        case TYPE_VOID:      return "void";
        case TYPE_UNKNOWN:   return "<unknown>";
    }
    return "<invalid>";
}

static int is_pointer_type(VarType t)
{
    return t == TYPE_INT_PTR || t == TYPE_FLOAT_PTR || t == TYPE_CHAR_PTR;
}

static int is_numeric_type(VarType t)
{
    return t == TYPE_INT || t == TYPE_FLOAT || t == TYPE_CHAR;
}

/* ════════════════════════════════════════════════════════════════
 *  Section 2: Symbol Table — Hash Map Implementation
 * ════════════════════════════════════════════════════════════════ */

#define SYMTAB_SIZE   64
#define MAX_SCOPES    16
#define MAX_NAME_LEN  32

typedef struct Symbol {
    char            name[MAX_NAME_LEN];
    VarType         type;
    int             scope_level;
    int             is_initialised;
    int             line_declared;      /* source line of declaration */
    struct Symbol  *next;               /* hash chain                */
} Symbol;

typedef struct {
    Symbol *buckets[SYMTAB_SIZE];
    int     current_scope;
    int     scope_stack[MAX_SCOPES];    /* track scope entry points  */
    int     scope_top;
    int     total_symbols;
    int     warnings;
    int     errors;
} SymbolTable;

/* djb2 hash */
static unsigned int hash_name(const char *name)
{
    unsigned int h = 5381;
    while (*name) {
        h = ((h << 5) + h) + (unsigned char)*name;
        name++;
    }
    return h % SYMTAB_SIZE;
}

static void symtab_init(SymbolTable *st)
{
    memset(st, 0, sizeof(SymbolTable));
    st->current_scope = 0;
    st->scope_top     = 0;
}

/* Look up a symbol by name — searches from current scope outward */
static Symbol *symtab_lookup(SymbolTable *st, const char *name)
{
    unsigned int idx = hash_name(name);
    Symbol *sym = st->buckets[idx];

    /* Find the most recent (highest scope) match */
    Symbol *best = NULL;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            if (!best || sym->scope_level > best->scope_level) {
                best = sym;
            }
        }
        sym = sym->next;
    }
    return best;
}

/* Look up a symbol in the CURRENT scope only */
static Symbol *symtab_lookup_current_scope(SymbolTable *st, const char *name)
{
    unsigned int idx = hash_name(name);
    Symbol *sym = st->buckets[idx];
    while (sym) {
        if (strcmp(sym->name, name) == 0 && sym->scope_level == st->current_scope)
            return sym;
        sym = sym->next;
    }
    return NULL;
}

/* Insert a new symbol */
static Symbol *symtab_insert(SymbolTable *st, const char *name, VarType type,
                              int initialised, int line)
{
    /* Check for redeclaration in same scope */
    Symbol *existing = symtab_lookup_current_scope(st, name);
    if (existing) {
        printf("    ❌ ERROR: '%s' already declared in this scope (line %d)\n",
               name, existing->line_declared);
        st->errors++;
        return NULL;
    }

    /* Check for shadowing */
    Symbol *outer = symtab_lookup(st, name);
    if (outer && outer->scope_level < st->current_scope) {
        printf("    ⚠  WARNING: '%s' shadows declaration from scope %d (line %d)\n",
               name, outer->scope_level, outer->line_declared);
        printf("       (gcc -Wshadow would warn about this)\n");
        st->warnings++;
    }

    /* Create and insert */
    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    strncpy(sym->name, name, MAX_NAME_LEN - 1);
    sym->name[MAX_NAME_LEN - 1] = '\0';
    sym->type            = type;
    sym->scope_level     = st->current_scope;
    sym->is_initialised  = initialised;
    sym->line_declared   = line;

    unsigned int idx = hash_name(name);
    sym->next            = st->buckets[idx];
    st->buckets[idx]     = sym;
    st->total_symbols++;

    printf("    ✓ Declared: %-8s %-6s  (scope=%d, line=%d, init=%s)\n",
           type_name(type), name, st->current_scope, line,
           initialised ? "yes" : "no");

    return sym;
}

/* Push a new scope */
static void symtab_push_scope(SymbolTable *st)
{
    if (st->scope_top < MAX_SCOPES) {
        st->scope_stack[st->scope_top++] = st->current_scope;
    }
    st->current_scope++;
    printf("    → Entering scope %d\n", st->current_scope);
}

/* Pop current scope — remove all symbols at this level */
static void symtab_pop_scope(SymbolTable *st)
{
    printf("    ← Leaving scope %d\n", st->current_scope);

    /* Remove symbols at current scope level */
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        Symbol **pp = &st->buckets[i];
        while (*pp) {
            if ((*pp)->scope_level == st->current_scope) {
                Symbol *doomed = *pp;
                *pp = doomed->next;
                free(doomed);
                st->total_symbols--;
            } else {
                pp = &(*pp)->next;
            }
        }
    }

    if (st->scope_top > 0) {
        st->current_scope = st->scope_stack[--st->scope_top];
    }
}

/* Free all remaining symbols */
static void symtab_free(SymbolTable *st)
{
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        Symbol *sym = st->buckets[i];
        while (sym) {
            Symbol *next = sym->next;
            free(sym);
            sym = next;
        }
        st->buckets[i] = NULL;
    }
    st->total_symbols = 0;
}

/* ════════════════════════════════════════════════════════════════
 *  Section 3: Type Checking Functions
 * ════════════════════════════════════════════════════════════════ */

/* Can we assign 'rhs_type' to a variable of 'lhs_type'? */
static void check_assignment_compat(SymbolTable *st,
                                     const char *var_name,
                                     VarType lhs_type,
                                     VarType rhs_type,
                                     int line)
{
    printf("    Checking: %s (%s) = <expr of type %s>  (line %d)\n",
           var_name, type_name(lhs_type), type_name(rhs_type), line);

    /* Same type — always OK */
    if (lhs_type == rhs_type) {
        printf("      ✓ Types match.\n");
        return;
    }

    /* Pointer to different pointer — ERROR */
    if (is_pointer_type(lhs_type) && is_pointer_type(rhs_type)) {
        printf("      ❌ ERROR: incompatible pointer types: %s ← %s\n",
               type_name(lhs_type), type_name(rhs_type));
        printf("         (gcc: 'incompatible pointer types')\n");
        st->errors++;
        return;
    }

    /* Pointer ← numeric or numeric ← pointer — ERROR */
    if ((is_pointer_type(lhs_type) && is_numeric_type(rhs_type)) ||
        (is_numeric_type(lhs_type) && is_pointer_type(rhs_type))) {
        printf("      ❌ ERROR: cannot assign %s to %s without cast\n",
               type_name(rhs_type), type_name(lhs_type));
        printf("         (gcc: 'assignment to pointer from integer without a cast')\n");
        st->errors++;
        return;
    }

    /* float → int — warning: possible data loss */
    if (lhs_type == TYPE_INT && rhs_type == TYPE_FLOAT) {
        printf("      ⚠  WARNING: implicit float → int conversion (data loss!)\n");
        printf("         (gcc -Wconversion would warn)\n");
        st->warnings++;
        return;
    }

    /* int → float — warning: possible precision loss for large ints */
    if (lhs_type == TYPE_FLOAT && rhs_type == TYPE_INT) {
        printf("      ⚠  WARNING: implicit int → float conversion\n");
        printf("         (gcc -Wconversion may warn for large values)\n");
        st->warnings++;
        return;
    }

    /* char → int or int → char */
    if ((lhs_type == TYPE_CHAR && rhs_type == TYPE_INT) ||
        (lhs_type == TYPE_INT && rhs_type == TYPE_CHAR)) {
        printf("      ⚠  WARNING: implicit %s → %s conversion\n",
               type_name(rhs_type), type_name(lhs_type));
        printf("         (gcc -Wconversion would warn)\n");
        st->warnings++;
        return;
    }

    /* Fallback for other numeric conversions */
    if (is_numeric_type(lhs_type) && is_numeric_type(rhs_type)) {
        printf("      ✓ Implicit numeric conversion: %s → %s\n",
               type_name(rhs_type), type_name(lhs_type));
        return;
    }

    printf("      ❌ ERROR: incompatible types: %s ← %s\n",
           type_name(lhs_type), type_name(rhs_type));
    st->errors++;
}

/* Check use of potentially uninitialised variable */
static void check_use(SymbolTable *st, const char *name, int line)
{
    Symbol *sym = symtab_lookup(st, name);
    if (!sym) {
        printf("    ❌ ERROR: use of undeclared identifier '%s' (line %d)\n",
               name, line);
        printf("       (gcc: 'undeclared identifier')\n");
        st->errors++;
        return;
    }
    if (!sym->is_initialised) {
        printf("    ⚠  WARNING: '%s' is used uninitialised (line %d)\n",
               name, line);
        printf("       (gcc -Wuninitialized would warn)\n");
        st->warnings++;
    }
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 1: Symbol Table Basics
 * ════════════════════════════════════════════════════════════════ */

static void demo_symbol_table(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 1: Symbol Table — Insert & Lookup            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("A symbol table maps names → {type, scope, attributes}.\n");
    printf("It's the compiler's \"phone book\" for variables.\n\n");

    printf("── Our implementation uses a hash map with chaining ──\n");
    printf("  Hash function: djb2 (fast, reasonable distribution)\n");
    printf("  %d buckets, separate chaining for collisions\n\n", SYMTAB_SIZE);

    SymbolTable st;
    symtab_init(&st);

    printf("── Simulating: ──\n");
    printf("  int x = 10;       // line 1\n");
    printf("  float y = 3.14;   // line 2\n");
    printf("  char ch;          // line 3 (uninitialised)\n");
    printf("  int *ptr = &x;    // line 4\n\n");

    symtab_insert(&st, "x",   TYPE_INT,       1, 1);
    symtab_insert(&st, "y",   TYPE_FLOAT,     1, 2);
    symtab_insert(&st, "ch",  TYPE_CHAR,      0, 3);
    symtab_insert(&st, "ptr", TYPE_INT_PTR,   1, 4);

    printf("\n── Lookups ──\n");
    Symbol *sx = symtab_lookup(&st, "x");
    Symbol *sy = symtab_lookup(&st, "y");
    Symbol *sz = symtab_lookup(&st, "z");

    printf("    lookup('x') → %s\n", sx ? type_name(sx->type) : "NOT FOUND");
    printf("    lookup('y') → %s\n", sy ? type_name(sy->type) : "NOT FOUND");
    printf("    lookup('z') → %s\n\n", sz ? type_name(sz->type) : "NOT FOUND");

    printf("── Check use of uninitialised 'ch' ──\n");
    check_use(&st, "ch", 5);
    printf("\n");

    symtab_free(&st);
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 2: Scope — Push/Pop and Shadowing
 * ════════════════════════════════════════════════════════════════ */

static void demo_scoping(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 2: Scope — Push/Pop and Shadowing            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("Simulating this code:\n\n");
    printf("  int x = 10;           // scope 0 (global)\n");
    printf("  int y = 20;           // scope 0\n");
    printf("  {                     // scope 1\n");
    printf("      float x = 3.14;  // shadows outer x!\n");
    printf("      int z = 30;      // local to scope 1\n");
    printf("      {                 // scope 2\n");
    printf("          char x = 'A';// shadows BOTH outer x's!\n");
    printf("      }                 // end scope 2\n");
    printf("  }                     // end scope 1\n");
    printf("  // z is gone, x is int again\n\n");

    SymbolTable st;
    symtab_init(&st);

    printf("── Scope 0 (global) ──\n");
    symtab_insert(&st, "x", TYPE_INT,   1, 1);
    symtab_insert(&st, "y", TYPE_INT,   1, 2);

    printf("\n── Enter scope 1 ──\n");
    symtab_push_scope(&st);
    symtab_insert(&st, "x", TYPE_FLOAT, 1, 4);   /* shadows outer x */
    symtab_insert(&st, "z", TYPE_INT,   1, 5);

    printf("\n    lookup('x') in scope 1:\n");
    Symbol *s1 = symtab_lookup(&st, "x");
    printf("      → type=%s, scope=%d  (the INNER one)\n\n",
           type_name(s1->type), s1->scope_level);

    printf("── Enter scope 2 ──\n");
    symtab_push_scope(&st);
    symtab_insert(&st, "x", TYPE_CHAR, 1, 7);    /* shadows again */

    printf("\n    lookup('x') in scope 2:\n");
    Symbol *s2 = symtab_lookup(&st, "x");
    printf("      → type=%s, scope=%d  (the INNERMOST one)\n\n",
           type_name(s2->type), s2->scope_level);

    printf("── Leave scope 2 ──\n");
    symtab_pop_scope(&st);

    printf("    lookup('x') back in scope 1:\n");
    Symbol *s3 = symtab_lookup(&st, "x");
    printf("      → type=%s, scope=%d  (float x is visible again)\n\n",
           type_name(s3->type), s3->scope_level);

    printf("── Leave scope 1 ──\n");
    symtab_pop_scope(&st);

    printf("    lookup('x') back in scope 0:\n");
    Symbol *s4 = symtab_lookup(&st, "x");
    printf("      → type=%s, scope=%d  (original int x)\n",
           type_name(s4->type), s4->scope_level);

    printf("    lookup('z') back in scope 0:\n");
    Symbol *s5 = symtab_lookup(&st, "z");
    printf("      → %s  (z was local to scope 1, now gone)\n\n",
           s5 ? type_name(s5->type) : "NOT FOUND");

    symtab_free(&st);
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 3: Type Checking
 * ════════════════════════════════════════════════════════════════ */

static void demo_type_checking(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 3: Type Checking — Detecting Mismatches      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("The semantic analyser checks that operations make sense.\n");
    printf("Parsing checked SYNTAX; now we check MEANING.\n\n");

    SymbolTable st;
    symtab_init(&st);

    printf("── Declarations ──\n");
    symtab_insert(&st, "count",  TYPE_INT,       1, 1);
    symtab_insert(&st, "ratio",  TYPE_FLOAT,     1, 2);
    symtab_insert(&st, "letter", TYPE_CHAR,      1, 3);
    symtab_insert(&st, "iptr",   TYPE_INT_PTR,   1, 4);
    symtab_insert(&st, "fptr",   TYPE_FLOAT_PTR, 1, 5);

    printf("\n── Assignment compatibility checks ──\n\n");

    /* Same type — OK */
    check_assignment_compat(&st, "count", TYPE_INT, TYPE_INT, 10);
    printf("\n");

    /* float → int — warning */
    check_assignment_compat(&st, "count", TYPE_INT, TYPE_FLOAT, 11);
    printf("\n");

    /* int → float — warning (precision) */
    check_assignment_compat(&st, "ratio", TYPE_FLOAT, TYPE_INT, 12);
    printf("\n");

    /* char → int — warning */
    check_assignment_compat(&st, "count", TYPE_INT, TYPE_CHAR, 13);
    printf("\n");

    /* int* ← float* — ERROR */
    check_assignment_compat(&st, "iptr", TYPE_INT_PTR, TYPE_FLOAT_PTR, 14);
    printf("\n");

    /* int* ← int — ERROR */
    check_assignment_compat(&st, "iptr", TYPE_INT_PTR, TYPE_INT, 15);
    printf("\n");

    /* Undeclared variable */
    check_use(&st, "missing_var", 16);
    printf("\n");

    printf("── Summary: %d error(s), %d warning(s) ──\n\n",
           st.errors, st.warnings);

    symtab_free(&st);
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 4: Implicit Conversions (Integer Promotion & Narrowing)
 * ════════════════════════════════════════════════════════════════ */

static void demo_implicit_conversions(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 4: Implicit Conversions & Promotions         ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("C performs many conversions silently. The semantic\n");
    printf("analyser tracks these to warn about data loss.\n\n");

    printf("── Integer promotion ──\n\n");
    printf("  In expressions, char and short are promoted to int:\n\n");
    char a = 100, b = 100;
    /* Integer promotion happens here: a and b become int */
    int result = a + b;
    printf("    char a = 100, b = 100;\n");
    printf("    int result = a + b;\n");
    printf("    result = %d  (not overflow! promoted to int first)\n\n", result);

    printf("  sizeof(a + b) = %zu  (int, not char!)\n\n",
           sizeof(a + b));

    printf("── Float ← int (widening — generally safe) ──\n\n");
    int big_int = 16777217;  /* 2^24 + 1 */
    float f = (float)big_int;
    printf("    int big_int = 16777217;  // 2^24 + 1\n");
    printf("    float f = big_int;\n");
    printf("    f = %.1f  (lost precision! float has 24-bit mantissa)\n",
           (double)f);
    printf("    Difference: %d\n\n", big_int - (int)f);

    printf("── int ← float (truncation — data loss) ──\n\n");
    float pi = 3.14159f;
    int truncated = (int)pi;
    printf("    float pi = 3.14159;\n");
    printf("    int truncated = pi;\n");
    printf("    truncated = %d  (fractional part lost!)\n\n", truncated);

    float big_float = 1e20f;
    printf("    float big = 1e20;\n");
    printf("    int x = big;  // UNDEFINED BEHAVIOUR if out of range!\n");
    printf("    (gcc -Wconversion warns about this)\n\n");
    (void)big_float;

    printf("── Usual arithmetic conversions ──\n\n");
    printf("  When operands differ, C converts to the \"wider\" type:\n\n");
    printf("  ┌──────────────────────┬────────────────────────────┐\n");
    printf("  │ Expression           │ Result Type                │\n");
    printf("  ├──────────────────────┼────────────────────────────┤\n");
    printf("  │ int + int            │ int                        │\n");
    printf("  │ int + float          │ float (int promoted)       │\n");
    printf("  │ float + double       │ double (float promoted)    │\n");
    printf("  │ char + char          │ int (integer promotion!)   │\n");
    printf("  │ int + unsigned int   │ unsigned int               │\n");
    printf("  │ long + int           │ long (int promoted)        │\n");
    printf("  └──────────────────────┴────────────────────────────┘\n\n");

    printf("  Danger: signed + unsigned can cause surprises!\n");
    int    si = -1;
    unsigned int ui = 1;
    printf("    int si = -1; unsigned int ui = 1;\n");
    printf("    (si < ui) is %s!\n",
           (si < ui) ? "true" : "false (SURPRISE! -1 becomes huge unsigned)");
    printf("    Because si is converted to unsigned: %u\n\n", (unsigned int)si);
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 5: GCC Semantic Warnings
 * ════════════════════════════════════════════════════════════════ */

static void demo_gcc_warnings(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 5: GCC Warnings = Semantic Checks            ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("GCC flags that enable semantic checking:\n\n");

    printf("  ┌─────────────────────┬──────────────────────────────────────┐\n");
    printf("  │ Flag                │ What it checks                       │\n");
    printf("  ├─────────────────────┼──────────────────────────────────────┤\n");
    printf("  │ -Wall               │ Common warnings (includes many)      │\n");
    printf("  │ -Wextra             │ Extra warnings beyond -Wall          │\n");
    printf("  │ -Wshadow            │ Variable shadows outer declaration   │\n");
    printf("  │ -Wconversion        │ Implicit type conversion may lose    │\n");
    printf("  │                     │ data (float→int, int→char, etc.)     │\n");
    printf("  │ -Wuninitialized     │ Using variable before assigning      │\n");
    printf("  │ -Wtype-limits       │ Comparison always true/false due to  │\n");
    printf("  │                     │ type limits (unsigned >= 0)          │\n");
    printf("  │ -Wsign-compare      │ Signed/unsigned comparison           │\n");
    printf("  │ -Wpointer-arith     │ Suspicious pointer arithmetic        │\n");
    printf("  │ -Wincompatible-     │ Pointer type mismatch in assignment  │\n");
    printf("  │  pointer-types      │                                      │\n");
    printf("  │ -Wimplicit-function │ Calling function without declaration │\n");
    printf("  │  -declaration       │ (removed in C23)                     │\n");
    printf("  │ -Wreturn-type       │ Function missing return, or wrong    │\n");
    printf("  │                     │ return type                          │\n");
    printf("  └─────────────────────┴──────────────────────────────────────┘\n\n");

    printf("── Example problematic code and what GCC says ──\n\n");

    printf("  Code: int x; printf(\"%%d\", x);\n");
    printf("  Flag: -Wuninitialized\n");
    printf("  Msg:  'x' is used uninitialized in this function\n\n");

    printf("  Code: int x = 10; { int x = 20; }\n");
    printf("  Flag: -Wshadow\n");
    printf("  Msg:  declaration of 'x' shadows a previous local\n\n");

    printf("  Code: int x = 3.14;\n");
    printf("  Flag: -Wconversion\n");
    printf("  Msg:  conversion from 'double' to 'int' may change value\n\n");

    printf("  Code: unsigned u = 0; if (u >= 0) ...\n");
    printf("  Flag: -Wtype-limits\n");
    printf("  Msg:  comparison of unsigned expression >= 0 is always true\n\n");

    printf("── Recommended compilation command ──\n\n");
    printf("  gcc -Wall -Wextra -Wshadow -Wconversion -Wpedantic \\\n");
    printf("      -std=c99 -o prog source.c\n\n");
    printf("  This catches the vast majority of semantic issues!\n\n");
}

/* ════════════════════════════════════════════════════════════════
 *  Demo 6: Full Scenario — Mini Semantic Analyser
 * ════════════════════════════════════════════════════════════════ */

static void demo_full_scenario(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Demo 6: Full Scenario — Analysing a Code Block    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("Analysing this pseudo-code:\n\n");
    printf("  1: int count = 0;\n");
    printf("  2: float average;\n");
    printf("  3: int *ptr = &count;\n");
    printf("  4: count = 42;             // OK: int = int\n");
    printf("  5: average = count;         // WARN: float = int\n");
    printf("  6: count = average;         // WARN: int = float\n");
    printf("  7: ptr = &average;          // ERROR: int* = float*\n");
    printf("  8: {\n");
    printf("  9:   float count = 1.5;     // WARN: shadows outer\n");
    printf(" 10:   printf(average);        // WARN: uninitialised?\n");
    printf(" 11: }\n");
    printf(" 12: x = 10;                  // ERROR: undeclared\n\n");

    printf("── Running semantic analysis ──\n\n");

    SymbolTable st;
    symtab_init(&st);

    /* Line 1-3: declarations */
    printf("  Lines 1-3: Declarations\n");
    symtab_insert(&st, "count",   TYPE_INT,       1, 1);
    symtab_insert(&st, "average", TYPE_FLOAT,     0, 2);  /* uninitialised */
    symtab_insert(&st, "ptr",     TYPE_INT_PTR,   1, 3);
    printf("\n");

    /* Line 4: count = 42 */
    printf("  Line 4: count = 42\n");
    check_assignment_compat(&st, "count", TYPE_INT, TYPE_INT, 4);
    printf("\n");

    /* Line 5: average = count */
    printf("  Line 5: average = count\n");
    check_use(&st, "count", 5);
    check_assignment_compat(&st, "average", TYPE_FLOAT, TYPE_INT, 5);
    /* Mark average as initialised now */
    Symbol *avg = symtab_lookup(&st, "average");
    if (avg) avg->is_initialised = 1;
    printf("\n");

    /* Line 6: count = average */
    printf("  Line 6: count = average\n");
    check_assignment_compat(&st, "count", TYPE_INT, TYPE_FLOAT, 6);
    printf("\n");

    /* Line 7: ptr = &average (type mismatch!) */
    printf("  Line 7: ptr = &average\n");
    check_assignment_compat(&st, "ptr", TYPE_INT_PTR, TYPE_FLOAT_PTR, 7);
    printf("\n");

    /* Line 8-11: inner scope */
    printf("  Line 8: Enter block\n");
    symtab_push_scope(&st);

    printf("\n  Line 9: float count = 1.5 (shadows!)\n");
    symtab_insert(&st, "count", TYPE_FLOAT, 1, 9);

    printf("\n  Line 10: use of 'average'\n");
    check_use(&st, "average", 10);

    printf("\n  Line 11: End block\n");
    symtab_pop_scope(&st);

    /* Line 12: undeclared variable */
    printf("\n  Line 12: x = 10\n");
    check_use(&st, "x", 12);

    printf("\n══════════════════════════════════════════════════════\n");
    printf("  Analysis complete: %d error(s), %d warning(s)\n",
           st.errors, st.warnings);
    printf("══════════════════════════════════════════════════════\n\n");

    symtab_free(&st);
}

/* ════════════════════════════════════════════════════════════════
 *  main
 * ════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 20 — Semantic Analysis                    ║\n");
    printf("║  Giving meaning to the syntax tree                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    printf("After parsing, the compiler has an AST but doesn't yet\n");
    printf("know if the code MAKES SENSE. Semantic analysis checks:\n");
    printf("  • Are variables declared before use?\n");
    printf("  • Do types match in operations?\n");
    printf("  • Are there scope/shadowing issues?\n");
    printf("  • Will implicit conversions lose data?\n\n");

    demo_symbol_table();
    demo_scoping();
    demo_type_checking();
    demo_implicit_conversions();
    demo_gcc_warnings();
    demo_full_scenario();

    printf("════════════════════════════════════════════════════════\n");
    printf(" Summary: AST + Symbol Table → type-checked, validated\n");
    printf("          program ready for code generation.\n");
    printf("════════════════════════════════════════════════════════\n\n");
    printf("  Semantic analysis is the compiler's \"sanity check\".\n");
    printf("  It catches bugs that syntax alone cannot detect.\n\n");
    printf("  Key GCC flags to try:\n");
    printf("    gcc -Wall -Wshadow -Wconversion -Wuninitialized\n\n");

    return 0;
}
