/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 21 — Intermediate Representation (IR)                  ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: Three-address code, SSA, GIMPLE, LLVM IR               ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What Is Intermediate Representation?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_what_is_ir(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What Is Intermediate Representation?       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("After parsing and semantic analysis, the compiler converts the AST\n");
    printf("into an Intermediate Representation (IR).  IR sits between the\n");
    printf("source language (C) and the target machine code (x86, ARM, etc.).\n\n");

    printf("Why use IR?\n");
    printf("  1. Machine-independence — optimisations work on any target.\n");
    printf("  2. Easier to optimise than source code or raw assembly.\n");
    printf("  3. Retargetable — same IR can produce x86, ARM, RISC-V code.\n");
    printf("  4. Cleaner analysis — complex language features are lowered\n");
    printf("     to simple primitives (assignments, gotos, calls).\n\n");

    printf("Common IR forms:\n");
    printf("  ┌─────────────────────┬──────────────────────────────┐\n");
    printf("  │ Form                │ Used By                      │\n");
    printf("  ├─────────────────────┼──────────────────────────────┤\n");
    printf("  │ Three-address code  │ Many textbook compilers      │\n");
    printf("  │ GIMPLE              │ GCC                          │\n");
    printf("  │ LLVM IR             │ Clang / LLVM                 │\n");
    printf("  │ RTL                 │ GCC (lower-level)            │\n");
    printf("  │ Bytecode            │ Java (JVM), Python (.pyc)    │\n");
    printf("  └─────────────────────┴──────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Three-Address Code
 * ════════════════════════════════════════════════════════════════════ */

/* A simple three-address instruction */
typedef enum { TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV, TAC_NEG,
               TAC_ASSIGN, TAC_LABEL, TAC_GOTO,
               TAC_IF_GT, TAC_IF_LT, TAC_IF_EQ } TacOp;

typedef struct {
    TacOp   op;
    char    result[16];
    char    arg1[16];
    char    arg2[16];
} TacInstr;

static void print_tac(const TacInstr *instr)
{
    switch (instr->op) {
    case TAC_ADD:
        printf("    %s = %s + %s\n", instr->result, instr->arg1, instr->arg2);
        break;
    case TAC_SUB:
        printf("    %s = %s - %s\n", instr->result, instr->arg1, instr->arg2);
        break;
    case TAC_MUL:
        printf("    %s = %s * %s\n", instr->result, instr->arg1, instr->arg2);
        break;
    case TAC_DIV:
        printf("    %s = %s / %s\n", instr->result, instr->arg1, instr->arg2);
        break;
    case TAC_NEG:
        printf("    %s = -%s\n", instr->result, instr->arg1);
        break;
    case TAC_ASSIGN:
        printf("    %s = %s\n", instr->result, instr->arg1);
        break;
    case TAC_LABEL:
        printf("  %s:\n", instr->result);
        break;
    case TAC_GOTO:
        printf("    goto %s\n", instr->result);
        break;
    case TAC_IF_GT:
        printf("    if %s > %s goto %s\n", instr->arg1, instr->arg2, instr->result);
        break;
    case TAC_IF_LT:
        printf("    if %s < %s goto %s\n", instr->arg1, instr->arg2, instr->result);
        break;
    case TAC_IF_EQ:
        printf("    if %s == %s goto %s\n", instr->arg1, instr->arg2, instr->result);
        break;
    }
}

static void demo_three_address_code(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Three-Address Code                         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Three-address code (TAC) uses at most three operands per instruction:\n");
    printf("    result = arg1  OP  arg2\n\n");

    /* Example 1: a = b + c * d */
    printf("── Example 1: a = b + c * d ──────────────────────────────\n\n");
    printf("  Source C:   a = b + c * d;\n\n");
    printf("  Three-address code:\n");
    TacInstr ex1[] = {
        { TAC_MUL,    "t1", "c",  "d"  },
        { TAC_ADD,    "t2", "b",  "t1" },
        { TAC_ASSIGN, "a",  "t2", ""   },
    };
    for (int i = 0; i < 3; i++) print_tac(&ex1[i]);

    /* Example 2: if (x > 0) y = x; else y = -x; */
    printf("\n── Example 2: if (x > 0) y = x; else y = -x; ───────────\n\n");
    printf("  Source C:   if (x > 0) y = x; else y = -x;\n\n");
    printf("  Three-address code:\n");
    TacInstr ex2[] = {
        { TAC_IF_GT,  "L_then", "x",  "0"  },
        { TAC_GOTO,   "L_else", "",    ""   },
        { TAC_LABEL,  "L_then", "",    ""   },
        { TAC_ASSIGN, "y",      "x",   ""   },
        { TAC_GOTO,   "L_end",  "",    ""   },
        { TAC_LABEL,  "L_else", "",    ""   },
        { TAC_NEG,    "t1",     "x",   ""   },
        { TAC_ASSIGN, "y",      "t1",  ""   },
        { TAC_LABEL,  "L_end",  "",    ""   },
    };
    for (int i = 0; i < 9; i++) print_tac(&ex2[i]);

    /* Example 3: for (i = 0; i < n; i++) sum += arr[i]; */
    printf("\n── Example 3: for-loop → goto-based IR ───────────────────\n\n");
    printf("  Source C:   for (i = 0; i < n; i++) sum += arr[i];\n\n");
    printf("  Three-address code:\n");
    TacInstr ex3[] = {
        { TAC_ASSIGN, "i",    "0",    ""    },
        { TAC_LABEL,  "L_top","",     ""    },
        { TAC_IF_LT,  "L_body","i",   "n"  },
        { TAC_GOTO,   "L_end","",     ""    },
        { TAC_LABEL,  "L_body","",    ""    },
        { TAC_MUL,    "t1",   "i",    "4"   },  /* offset = i*sizeof(int) */
        { TAC_ADD,    "t2",   "arr",  "t1"  },  /* address = arr + offset */
        { TAC_ASSIGN, "t3",   "*t2",  ""    },  /* t3 = arr[i] */
        { TAC_ADD,    "sum",  "sum",  "t3"  },
        { TAC_ADD,    "i",    "i",    "1"   },
        { TAC_GOTO,   "L_top","",     ""    },
        { TAC_LABEL,  "L_end","",     ""    },
    };
    for (int i = 0; i < 12; i++) print_tac(&ex3[i]);
    printf("\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Static Single Assignment (SSA) Form
 * ════════════════════════════════════════════════════════════════════ */
static void demo_ssa(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Static Single Assignment (SSA) Form        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("In SSA form, every variable is assigned exactly once.\n");
    printf("When a variable could come from two paths, a phi-node merges them.\n\n");

    printf("── Before SSA ─────────────────────────────────────────────\n");
    printf("    x = 1\n");
    printf("    x = x + 1      ← 'x' assigned twice\n");
    printf("    y = x * 2\n\n");

    printf("── After SSA ──────────────────────────────────────────────\n");
    printf("    x1 = 1\n");
    printf("    x2 = x1 + 1    ← each 'x' is unique\n");
    printf("    y1 = x2 * 2\n\n");

    printf("── Phi-node example ───────────────────────────────────────\n\n");
    printf("  Source C:   if (cond) x = 1; else x = 2;\n");
    printf("              y = x + 3;\n\n");
    printf("  SSA form:\n");
    printf("    if (cond) goto L_then\n");
    printf("    goto L_else\n");
    printf("  L_then:\n");
    printf("    x1 = 1\n");
    printf("    goto L_merge\n");
    printf("  L_else:\n");
    printf("    x2 = 2\n");
    printf("    goto L_merge\n");
    printf("  L_merge:\n");
    printf("    x3 = phi(x1, x2)      ← phi picks based on predecessor\n");
    printf("    y1 = x3 + 3\n\n");

    printf("Why SSA?\n");
    printf("  - Each use has exactly one reaching definition → simpler analysis.\n");
    printf("  - Enables constant propagation, dead code elimination,\n");
    printf("    and many other optimisations more efficiently.\n");
    printf("  - Used by both GCC (GIMPLE-SSA) and LLVM IR.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — GCC GIMPLE
 * ════════════════════════════════════════════════════════════════════ */
static void demo_gimple(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — GCC GIMPLE                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("GIMPLE is GCC's primary IR.  It lowers C/C++ into simple\n");
    printf("three-address statements with at most 3 operands.\n\n");

    printf("Example — GCC GIMPLE output for: int f(int a, int b) { return a+b*2; }\n\n");
    printf("  f (int a, int b)\n");
    printf("  {\n");
    printf("    int D.1234;\n");
    printf("    int _1;\n");
    printf("    _1 = b * 2;\n");
    printf("    D.1234 = a + _1;\n");
    printf("    return D.1234;\n");
    printf("  }\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Try it yourself:                                       ║\n");
    printf("║                                                         ║\n");
    printf("║  # See GIMPLE output:                                   ║\n");
    printf("║  gcc -fdump-tree-gimple -c intermediate_repr.c          ║\n");
    printf("║  cat intermediate_repr.c.004t.gimple                    ║\n");
    printf("║                                                         ║\n");
    printf("║  # See SSA form:                                        ║\n");
    printf("║  gcc -O2 -fdump-tree-ssa -c intermediate_repr.c         ║\n");
    printf("║  cat intermediate_repr.c.*.ssa                          ║\n");
    printf("║                                                         ║\n");
    printf("║  # See all GIMPLE passes:                               ║\n");
    printf("║  gcc -O2 -fdump-tree-all -c intermediate_repr.c         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — LLVM IR
 * ════════════════════════════════════════════════════════════════════ */
static void demo_llvm_ir(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — LLVM IR                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("LLVM IR is the intermediate form used by Clang/LLVM.\n");
    printf("It is already in SSA form and uses typed virtual registers.\n\n");

    printf("Example — LLVM IR for: int add(int a, int b) { return a + b; }\n\n");
    printf("  define i32 @add(i32 %%a, i32 %%b) {\n");
    printf("  entry:\n");
    printf("    %%result = add i32 %%a, %%b\n");
    printf("    ret i32 %%result\n");
    printf("  }\n\n");

    printf("Key differences between GIMPLE and LLVM IR:\n");
    printf("  ┌──────────────────────┬─────────────────┬────────────────┐\n");
    printf("  │ Feature              │ GIMPLE (GCC)    │ LLVM IR        │\n");
    printf("  ├──────────────────────┼─────────────────┼────────────────┤\n");
    printf("  │ SSA by default       │ After SSA pass  │ Always SSA     │\n");
    printf("  │ Type system          │ C types         │ Own type system│\n");
    printf("  │ Human-readable       │ Dump file       │ .ll file       │\n");
    printf("  │ Bitcode format       │ No              │ .bc file       │\n");
    printf("  │ Serialisable         │ LTO only        │ Full support   │\n");
    printf("  └──────────────────────┴─────────────────┴────────────────┘\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Try it (requires clang):                               ║\n");
    printf("║                                                         ║\n");
    printf("║  clang -emit-llvm -S -o - intermediate_repr.c           ║\n");
    printf("║  clang -emit-llvm -c -o output.bc intermediate_repr.c   ║\n");
    printf("║  llvm-dis output.bc -o output.ll                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — GCC's Multi-Level IR Pipeline
 * ════════════════════════════════════════════════════════════════════ */
static void demo_gcc_pipeline(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — GCC's Multi-Level IR Pipeline              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("GCC uses multiple levels of IR:\n\n");

    printf("  Source Code (C/C++/Fortran/...)\n");
    printf("        │\n");
    printf("        ▼\n");
    printf("  ┌──────────────────┐\n");
    printf("  │  GENERIC         │  Language-specific AST, lowered to\n");
    printf("  └──────────────────┘  a common tree representation.\n");
    printf("        │\n");
    printf("        ▼\n");
    printf("  ┌──────────────────┐\n");
    printf("  │  GIMPLE          │  Three-address code, ~200 passes\n");
    printf("  └──────────────────┘  (constant prop, inlining, vectorise)\n");
    printf("        │\n");
    printf("        ▼\n");
    printf("  ┌──────────────────┐\n");
    printf("  │  GIMPLE-SSA      │  SSA form of GIMPLE for advanced opts.\n");
    printf("  └──────────────────┘\n");
    printf("        │\n");
    printf("        ▼\n");
    printf("  ┌──────────────────┐\n");
    printf("  │  RTL             │  Register Transfer Language —\n");
    printf("  └──────────────────┘  machine-dependent, close to assembly.\n");
    printf("        │\n");
    printf("        ▼\n");
    printf("  ┌──────────────────┐\n");
    printf("  │  Assembly (.s)   │  Target-specific instructions.\n");
    printf("  └──────────────────┘\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  See all passes:                                        ║\n");
    printf("║  gcc -O2 -fdump-tree-all -fdump-rtl-all -c file.c       ║\n");
    printf("║  ls *.gimple *.ssa *.expand *.final                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 21 — Intermediate Representation (IR)              ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_what_is_ir();
    demo_three_address_code();
    demo_ssa();
    demo_gimple();
    demo_llvm_ir();
    demo_gcc_pipeline();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 21 — Intermediate Representation\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
