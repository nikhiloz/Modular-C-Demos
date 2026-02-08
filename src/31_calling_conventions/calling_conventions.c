/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 31 — Stack Frames & Calling Conventions                ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: System V ABI, stack frames, varargs, red zone          ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What Is a Calling Convention?
 * ════════════════════════════════════════════════════════════════════ */
static void demo_calling_conv(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What Is a Calling Convention?              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("A calling convention is the agreement between caller and\n");
    printf("callee about:\n\n");

    printf("  1. How arguments are passed (registers? stack?)\n");
    printf("  2. How the return value is delivered.\n");
    printf("  3. Which registers the callee may destroy (caller-saved)\n");
    printf("     vs which it must preserve (callee-saved).\n");
    printf("  4. How the stack is maintained (alignment, cleanup).\n\n");

    printf("  Common conventions:\n");
    printf("  ┌──────────────────────┬─────────────────────────────────┐\n");
    printf("  │ Convention           │ Used By                         │\n");
    printf("  ├──────────────────────┼─────────────────────────────────┤\n");
    printf("  │ System V AMD64 ABI   │ Linux, macOS, BSD (x86-64)     │\n");
    printf("  │ Microsoft x64        │ Windows (x86-64)               │\n");
    printf("  │ AAPCS                │ ARM (32-bit)                    │\n");
    printf("  │ AAPCS64              │ AArch64 (ARM 64-bit)           │\n");
    printf("  │ cdecl                │ Legacy x86 (32-bit) C          │\n");
    printf("  │ stdcall              │ Win32 API                      │\n");
    printf("  └──────────────────────┴─────────────────────────────────┘\n\n");

    printf("  This chapter focuses on System V AMD64 ABI (Linux x86-64).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — System V x86-64 ABI — Parameter Passing
 * ════════════════════════════════════════════════════════════════════ */

/* Functions to demonstrate argument passing */
int args_6(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}

int args_8(int a, int b, int c, int d, int e, int f, int g, int h)
{
    return a + b + c + d + e + f + g + h;
}

static void demo_param_passing(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — System V x86-64: Parameter Passing         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Integer/pointer arguments → registers:\n");
    printf("    Arg 1: rdi      Arg 2: rsi      Arg 3: rdx\n");
    printf("    Arg 4: rcx      Arg 5: r8       Arg 6: r9\n");
    printf("    Arg 7+: stack (right-to-left push order)\n\n");

    printf("  Floating-point arguments → SSE registers:\n");
    printf("    xmm0, xmm1, xmm2, ..., xmm7 (up to 8)\n\n");

    printf("  Return value:\n");
    printf("    Integer: rax (up to 128 bits: rax + rdx)\n");
    printf("    Float:   xmm0 (up to 2 values: xmm0 + xmm1)\n");
    printf("    Structs > 16 bytes: caller passes hidden pointer in rdi\n\n");

    printf("  Demo — 6 args (all registers):\n");
    printf("    args_6(1,2,3,4,5,6) = %d  (expected 21)\n", args_6(1,2,3,4,5,6));
    printf("    → rdi=1, rsi=2, rdx=3, rcx=4, r8=5, r9=6\n\n");

    printf("  Demo — 8 args (6 regs + 2 on stack):\n");
    printf("    args_8(1,2,3,4,5,6,7,8) = %d  (expected 36)\n",
           args_8(1,2,3,4,5,6,7,8));
    printf("    → rdi..r9 for first 6, push 8 then push 7 (RTL)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Stack Frame Layout
 * ════════════════════════════════════════════════════════════════════ */

/* Nested functions to demonstrate frame chain */
static void inner(int x)
{
    int local_inner = x * 2;
    printf("    inner: &local_inner = %p (val=%d)\n",
           (void*)&local_inner, local_inner);
}

static void outer(int a, int b)
{
    int local_outer = a + b;
    printf("    outer: &local_outer = %p (val=%d)\n",
           (void*)&local_outer, local_outer);
    inner(local_outer);
}

static void demo_stack_frame(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Stack Frame Layout                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Each function call creates a stack frame:\n\n");

    printf("  High address\n");
    printf("  ┌─────────────────────────┐\n");
    printf("  │ Caller's frame          │\n");
    printf("  ├─────────────────────────┤\n");
    printf("  │ Stack arguments (7+)    │ If more than 6 args\n");
    printf("  ├─────────────────────────┤\n");
    printf("  │ Return address          │ Pushed by 'call' instruction\n");
    printf("  ├─────────────────────────┤ ← rbp (if frame pointer used)\n");
    printf("  │ Saved rbp               │\n");
    printf("  ├─────────────────────────┤\n");
    printf("  │ Callee-saved registers  │ rbx, r12-r15 (if used)\n");
    printf("  ├─────────────────────────┤\n");
    printf("  │ Local variables          │\n");
    printf("  ├─────────────────────────┤\n");
    printf("  │ Args for next call       │ (if needed)\n");
    printf("  ├─────────────────────────┤ ← rsp (16-byte aligned)\n");
    printf("  │ Red zone (128 bytes)    │ Below rsp, usable by leaf funcs\n");
    printf("  └─────────────────────────┘\n");
    printf("  Low address\n\n");

    printf("  Live demo — nesting two functions:\n");
    outer(10, 20);
    printf("  (Notice: inner's locals are at lower address than outer's.)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Callee-Saved vs Caller-Saved Registers
 * ════════════════════════════════════════════════════════════════════ */
static void demo_saved_registers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Callee-Saved vs Caller-Saved Registers     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ┌──────────────┬───────────────────────────────────────┐\n");
    printf("  │ Register     │ Responsibility                        │\n");
    printf("  ├──────────────┼───────────────────────────────────────┤\n");
    printf("  │ rax          │ Caller-saved (return value)           │\n");
    printf("  │ rcx, rdx     │ Caller-saved (args 4, 3)             │\n");
    printf("  │ rsi, rdi     │ Caller-saved (args 2, 1)             │\n");
    printf("  │ r8 - r11     │ Caller-saved (args 5-6 + scratch)    │\n");
    printf("  ├──────────────┼───────────────────────────────────────┤\n");
    printf("  │ rbx          │ Callee-saved (must preserve)          │\n");
    printf("  │ rbp          │ Callee-saved (frame pointer)          │\n");
    printf("  │ r12 - r15    │ Callee-saved (must preserve)          │\n");
    printf("  │ rsp          │ Callee-saved (stack pointer)          │\n");
    printf("  └──────────────┴───────────────────────────────────────┘\n\n");

    printf("  Caller-saved: the calling function must save these before\n");
    printf("    making a call if it needs the values afterwards.\n\n");
    printf("  Callee-saved: the called function must save (push) these\n");
    printf("    if it wants to use them, and restore (pop) before returning.\n\n");

    printf("  In practice:\n");
    printf("    - Simple leaf functions may use only caller-saved regs.\n");
    printf("    - Complex functions push callee-saved regs in prologue.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — The Red Zone
 * ════════════════════════════════════════════════════════════════════ */
static void demo_red_zone(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — The Red Zone                               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The System V x86-64 ABI reserves 128 bytes BELOW rsp:\n");
    printf("the 'red zone'. Leaf functions can use this space without\n");
    printf("adjusting rsp.\n\n");

    printf("     rsp → ┌───────────────────┐\n");
    printf("           │                   │\n");
    printf("           │ Red zone          │  128 bytes\n");
    printf("           │ (safe from signals │\n");
    printf("           │  and interrupts)   │\n");
    printf("  rsp-128 →└───────────────────┘\n\n");

    printf("  Benefit: leaf functions skip prologue/epilogue entirely.\n");
    printf("  The compiler can store locals in the red zone.\n\n");

    printf("  Caveat:\n");
    printf("    - Only for user-mode code (kernel code cannot use it).\n");
    printf("    - Signal handlers may clobber it (kernel adjusts rsp).\n");
    printf("    - Windows x64 does NOT have a red zone.\n\n");

    printf("  Disable: gcc -mno-red-zone (needed for kernel code)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Variadic Functions (va_list Internals)
 * ════════════════════════════════════════════════════════════════════ */

static int my_sum(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    int total = 0;
    for (int i = 0; i < count; i++)
        total += va_arg(ap, int);
    va_end(ap);
    return total;
}

static void demo_varargs(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Variadic Functions                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Variadic functions (printf, etc.) use special ABI handling.\n\n");

    printf("  On System V x86-64:\n");
    printf("    - Register arguments are saved to a 'register save area'\n");
    printf("      on the stack by the called function.\n");
    printf("    - va_list contains:\n");
    printf("        gp_offset  — next general-purpose reg to use\n");
    printf("        fp_offset  — next floating-point reg to use\n");
    printf("        overflow_arg_area — stack args pointer\n");
    printf("        reg_save_area    — saved register args pointer\n\n");

    printf("    - al register = number of vector (SSE) register args.\n");
    printf("      (That's why printf needs the prototype!)\n\n");

    printf("  Demo: my_sum(4, 10, 20, 30, 40) = %d\n\n", my_sum(4, 10, 20, 30, 40));

    printf("  Implementation of va_arg:\n");
    printf("    if (gp_offset < 48)  → fetch from reg_save_area + gp_offset\n");
    printf("    else                 → fetch from overflow_arg_area (stack)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Stack Alignment
 * ════════════════════════════════════════════════════════════════════ */
static void demo_alignment(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Stack Alignment                             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The System V ABI requires the stack to be 16-byte aligned\n");
    printf("at the point of a CALL instruction.\n\n");

    printf("  Before call:  rsp %% 16 == 0\n");
    printf("  After call:   rsp %% 16 == 8  (return address pushed)\n");
    printf("  After push rbp: rsp %% 16 == 0  (aligned again)\n\n");

    printf("  Why 16-byte alignment?\n");
    printf("    SSE instructions (movaps, etc.) require 16-byte alignment.\n");
    printf("    Misaligned SSE access → SIGSEGV!\n\n");

    /* Verify current stack alignment */
    int local;
    uintptr_t addr = (uintptr_t)&local;
    printf("  Demo: &local = %p, addr %% 16 = %lu\n\n", (void*)&local, addr % 16);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Examining Stack Frames
 * ════════════════════════════════════════════════════════════════════ */
static void demo_examine(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Examining Stack Frames                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  See function prologue/epilogue:                        ║\n");
    printf("║  gcc -S -masm=intel -O0 calling_conventions.c           ║\n");
    printf("║  grep -A10 'outer:' calling_conventions.s               ║\n");
    printf("║                                                         ║\n");
    printf("║  GDB: view stack frames:                                ║\n");
    printf("║  gdb ./calling_conventions                              ║\n");
    printf("║    (gdb) break inner                                    ║\n");
    printf("║    (gdb) run                                            ║\n");
    printf("║    (gdb) bt          # backtrace                        ║\n");
    printf("║    (gdb) info frame  # current frame details            ║\n");
    printf("║    (gdb) info registers                                 ║\n");
    printf("║    (gdb) x/20gx $rsp # examine stack memory             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 31 — Stack Frames & Calling Conventions            ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_calling_conv();
    demo_param_passing();
    demo_stack_frame();
    demo_saved_registers();
    demo_red_zone();
    demo_varargs();
    demo_alignment();
    demo_examine();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 31 — Stack Frames & Calling Conventions\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
