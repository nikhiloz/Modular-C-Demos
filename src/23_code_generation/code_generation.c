/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 23 — Code Generation                                   ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: Register alloc, instruction selection, calling conv    ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ════════════════════════════════════════════════════════════════════
 *  Example functions — designed to produce readable assembly
 *  Compile with: gcc -S -masm=intel -O0 code_generation.c
 * ════════════════════════════════════════════════════════════════════ */

/* Simple arithmetic — maps to mov + add + ret */
int simple_add(int a, int b)
{
    return a + b;
}

/* Array sum — loop with memory access */
int array_sum(const int *arr, int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    return sum;
}

/* Conditional — branch instruction */
int abs_val(int x)
{
    if (x < 0)
        return -x;
    return x;
}

/* Pointer swap — memory load/store */
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Multiple parameters — shows register usage */
int multi_param(int a, int b, int c, int d, int e, int f)
{
    return a + b - c * d + e - f;
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What Is Code Generation?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_what_is_codegen(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What Is Code Generation?                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Code generation is the final stage where optimised IR is\n");
    printf("translated into target machine instructions.\n\n");

    printf("Key tasks:\n");
    printf("  1. Instruction Selection — pick machine instructions\n");
    printf("  2. Register Allocation  — map variables to registers\n");
    printf("  3. Instruction Scheduling — reorder for pipeline\n");
    printf("  4. Frame Layout — decide stack frame structure\n\n");

    printf("  Optimised IR  →  [Instruction Selection]  →  Virtual regs\n");
    printf("                →  [Register Allocation]    →  Physical regs\n");
    printf("                →  [Scheduling]             →  Final .s file\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — x86-64 Register Overview
 * ════════════════════════════════════════════════════════════════════ */
static void demo_registers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — x86-64 Register Overview                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  x86-64 (System V AMD64 ABI — used on Linux):\n\n");
    printf("  ┌──────────┬───────────────────────────────────────────┐\n");
    printf("  │ Register │ Purpose                                   │\n");
    printf("  ├──────────┼───────────────────────────────────────────┤\n");
    printf("  │ rax      │ Return value                              │\n");
    printf("  │ rdi      │ 1st integer argument                      │\n");
    printf("  │ rsi      │ 2nd integer argument                      │\n");
    printf("  │ rdx      │ 3rd integer argument                      │\n");
    printf("  │ rcx      │ 4th integer argument                      │\n");
    printf("  │ r8       │ 5th integer argument                      │\n");
    printf("  │ r9       │ 6th integer argument                      │\n");
    printf("  │ rsp      │ Stack pointer                             │\n");
    printf("  │ rbp      │ Frame pointer (base pointer)              │\n");
    printf("  │ rbx      │ Callee-saved (must preserve)              │\n");
    printf("  │ r12-r15  │ Callee-saved (must preserve)              │\n");
    printf("  │ r10, r11 │ Caller-saved (scratch)                    │\n");
    printf("  └──────────┴───────────────────────────────────────────┘\n\n");

    printf("  Arguments 7+ are passed on the stack.\n");
    printf("  Floating-point arguments use xmm0-xmm7.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Function Prologue & Epilogue
 * ════════════════════════════════════════════════════════════════════ */
static void demo_prologue_epilogue(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Function Prologue & Epilogue               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Every function starts with a prologue and ends with an epilogue.\n\n");

    printf("  Prologue (set up stack frame):\n");
    printf("    push  rbp              ; save old frame pointer\n");
    printf("    mov   rbp, rsp         ; establish new frame\n");
    printf("    sub   rsp, 32          ; allocate local space\n\n");

    printf("  Epilogue (tear down):\n");
    printf("    mov   rsp, rbp         ; deallocate locals\n");
    printf("    pop   rbp              ; restore old frame\n");
    printf("    ret                    ; return to caller\n\n");

    printf("  Note: At -O2, GCC often omits the frame pointer\n");
    printf("  (-fomit-frame-pointer) and uses rsp-relative addressing.\n\n");

    printf("  Stack frame layout:\n");
    printf("       ┌─────────────────────┐  ← high address\n");
    printf("       │ Return address       │\n");
    printf("       ├─────────────────────┤\n");
    printf("       │ Saved rbp            │ ← rbp points here\n");
    printf("       ├─────────────────────┤\n");
    printf("       │ Local variable 1     │ rbp - 4\n");
    printf("       │ Local variable 2     │ rbp - 8\n");
    printf("       │ ...                  │\n");
    printf("       ├─────────────────────┤\n");
    printf("       │ (alignment padding)  │ ← rsp (16-byte aligned)\n");
    printf("       └─────────────────────┘  ← low address\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Instruction Selection Examples
 * ════════════════════════════════════════════════════════════════════ */
static void demo_instruction_selection(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Instruction Selection Examples              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("── simple_add(int a, int b) ──────────────────────────────\n\n");
    printf("  C code:     return a + b;\n\n");
    printf("  Expected assembly (Intel syntax, -O2):\n");
    printf("    lea   eax, [rdi + rsi]     ; result = a + b\n");
    printf("    ret\n\n");

    printf("  Demo: simple_add(10, 25) = %d\n\n", simple_add(10, 25));

    printf("── abs_val(int x) ────────────────────────────────────────\n\n");
    printf("  C code:     if (x < 0) return -x; return x;\n\n");
    printf("  Expected assembly:\n");
    printf("    mov   eax, edi             ; eax = x\n");
    printf("    neg   eax                  ; eax = -x\n");
    printf("    cmovs eax, edi             ; if negative result, use original\n");
    printf("    ret\n\n");

    printf("  Demo: abs_val(-42) = %d\n", abs_val(-42));
    printf("  Demo: abs_val(17)  = %d\n\n", abs_val(17));

    printf("── swap(int *a, int *b) ──────────────────────────────────\n\n");
    printf("  C code:     int tmp = *a; *a = *b; *b = tmp;\n\n");
    printf("  Expected assembly:\n");
    printf("    mov   eax, [rdi]           ; tmp = *a\n");
    printf("    mov   edx, [rsi]           ; load *b\n");
    printf("    mov   [rdi], edx           ; *a = *b\n");
    printf("    mov   [rsi], eax           ; *b = tmp\n");
    printf("    ret\n\n");

    int x = 10, y = 20;
    printf("  Demo: before swap: x=%d, y=%d\n", x, y);
    swap(&x, &y);
    printf("  Demo: after  swap: x=%d, y=%d\n\n", x, y);

    printf("── multi_param(a,b,c,d,e,f) ─────────────────────────────\n\n");
    printf("  6 parameters → all in registers (rdi,rsi,rdx,rcx,r8,r9)\n");
    printf("  Demo: multi_param(1,2,3,4,5,6) = %d\n", multi_param(1,2,3,4,5,6));
    printf("  Expected: 1+2-3*4+5-6 = 1+2-12+5-6 = -10\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Register Allocation
 * ════════════════════════════════════════════════════════════════════ */
static void demo_register_allocation(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Register Allocation                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The compiler maps virtual registers (unlimited) to physical\n");
    printf("registers (limited: ~16 on x86-64).\n\n");

    printf("Algorithm sketch (graph colouring):\n");
    printf("  1. Build interference graph — two vars that are live at the\n");
    printf("     same time cannot share a register (they interfere).\n");
    printf("  2. Colour the graph with K colours (K = number of registers).\n");
    printf("  3. If a variable can't be coloured → spill to stack memory.\n\n");

    printf("  Example — 3 variables, 2 registers:\n");
    printf("    a = 1;        ← a is live\n");
    printf("    b = 2;        ← a, b are live → must use different regs\n");
    printf("    c = a + b;    ← a, b consumed; c alive\n");
    printf("    return c;\n\n");
    printf("    a → reg1, b → reg2, c → reg1 (a is dead by then)\n\n");

    printf("  Spilling:\n");
    printf("    When we run out of registers, the compiler spills a variable\n");
    printf("    to the stack frame, loading it back when needed.\n");
    printf("    Spilling is expensive → good allocation is critical.\n\n");

    printf("  Modern compilers (GCC, LLVM) use more sophisticated algorithms:\n");
    printf("    GCC:  IRA (Integrated Register Allocator)\n");
    printf("    LLVM: Greedy register allocator with live range splitting\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Array Sum Assembly Walkthrough
 * ════════════════════════════════════════════════════════════════════ */
static void demo_array_sum_codegen(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Array Sum Assembly Walkthrough              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    int data[] = {10, 20, 30, 40, 50};
    int result = array_sum(data, 5);

    printf("  C code:\n");
    printf("    int array_sum(const int *arr, int n) {\n");
    printf("        int sum = 0;\n");
    printf("        for (int i = 0; i < n; i++)\n");
    printf("            sum += arr[i];\n");
    printf("        return sum;\n");
    printf("    }\n\n");

    printf("  Expected x86-64 assembly (-O2, Intel syntax):\n");
    printf("    array_sum:\n");
    printf("        xor   eax, eax           ; sum = 0\n");
    printf("        test  esi, esi           ; if n <= 0\n");
    printf("        jle   .done             ; skip loop\n");
    printf("        movsxd rcx, esi          ; rcx = n (sign-extend)\n");
    printf("        xor   edx, edx           ; i = 0\n");
    printf("    .loop:\n");
    printf("        add   eax, [rdi+rdx*4]   ; sum += arr[i]\n");
    printf("        inc   rdx                ; i++\n");
    printf("        cmp   rdx, rcx           ; i < n?\n");
    printf("        jl    .loop\n");
    printf("    .done:\n");
    printf("        ret                      ; return sum in eax\n\n");

    printf("  Demo: array_sum({10,20,30,40,50}, 5) = %d\n\n", result);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Try It Yourself
 * ════════════════════════════════════════════════════════════════════ */
static void demo_try_it(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Try It Yourself                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Generate assembly:                                     ║\n");
    printf("║  gcc -S -masm=intel -O0 -o cg_O0.s code_generation.c   ║\n");
    printf("║  gcc -S -masm=intel -O2 -o cg_O2.s code_generation.c   ║\n");
    printf("║                                                         ║\n");
    printf("║  Compare O0 vs O2:                                      ║\n");
    printf("║  diff cg_O0.s cg_O2.s                                   ║\n");
    printf("║                                                         ║\n");
    printf("║  Disassemble compiled object:                           ║\n");
    printf("║  gcc -c -O2 code_generation.c                           ║\n");
    printf("║  objdump -d -M intel code_generation.o                   ║\n");
    printf("║                                                         ║\n");
    printf("║  See register allocation spills:                        ║\n");
    printf("║  gcc -O2 -fdump-rtl-ira -c code_generation.c            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 23 — Code Generation                               ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_what_is_codegen();
    demo_registers();
    demo_prologue_epilogue();
    demo_instruction_selection();
    demo_register_allocation();
    demo_array_sum_codegen();
    demo_try_it();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 23 — Code Generation\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
