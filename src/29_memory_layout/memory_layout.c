/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 29 — Process Memory Layout                             ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: text, data, bss, heap, stack, mmap, /proc/self/maps   ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/* ── Variables in different memory regions ────────────────────── */

/* .text — code (this function lives here) */
/* .rodata — read-only data */
const char   ro_msg[]    = "I live in .rodata";
const int    ro_array[3] = {10, 20, 30};

/* .data — initialised global/static data */
int          rw_global  = 42;
static int   rw_static  = 99;

/* .bss — uninitialised (zero-initialised) data */
int          bss_global;
static int   bss_static;
static char  bss_buffer[4096];

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Process Memory Map Overview
 * ════════════════════════════════════════════════════════════════════ */
static void demo_memory_map(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Process Memory Map Overview                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Virtual address space layout (x86-64 Linux):\n\n");

    printf("  0xFFFF'FFFF'FFFF'FFFF ┌────────────────────────┐\n");
    printf("                         │  Kernel space           │\n");
    printf("  0x0000'7FFF'FFFF'FFFF ├────────────────────────┤\n");
    printf("                         │                        │\n");
    printf("                         │  Stack  ↓              │ grows downward\n");
    printf("                         │         ...            │\n");
    printf("                         │  (guard page)          │\n");
    printf("                         │         ...            │\n");
    printf("                         │  mmap region           │ shared libs, mmap\n");
    printf("                         │         ...            │\n");
    printf("                         │  (free space)          │\n");
    printf("                         │         ...            │\n");
    printf("                         │  Heap   ↑              │ grows upward\n");
    printf("                         │  [brk]                 │ sbrk/brk boundary\n");
    printf("                         ├────────────────────────┤\n");
    printf("                         │  .bss  (zero-init)     │ RW-\n");
    printf("                         │  .data (init globals)  │ RW-\n");
    printf("                         │  .rodata (constants)   │ R--\n");
    printf("                         │  .text  (code)         │ R-X\n");
    printf("                         ├────────────────────────┤\n");
    printf("                         │  ELF headers           │\n");
    printf("  0x0000'0000'0040'0000 └────────────────────────┘\n");
    printf("  (or randomised with ASLR/PIE)\n\n");

    printf("  Each region has specific permissions (enforced by MMU):\n");
    printf("    .text   → Read + Execute      (no write = W^X security)\n");
    printf("    .rodata → Read only\n");
    printf("    .data   → Read + Write\n");
    printf("    .bss    → Read + Write\n");
    printf("    heap    → Read + Write\n");
    printf("    stack   → Read + Write\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Live Address Demo
 * ════════════════════════════════════════════════════════════════════ */
static void demo_live_addresses(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Live Address Demo                          ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* Stack variables */
    int         stack_var  = 123;
    char        stack_buf[64];
    strcpy(stack_buf, "on the stack");

    /* Heap allocation */
    int *heap_ptr = malloc(sizeof(int) * 10);
    heap_ptr[0] = 999;

    /* mmap allocation */
    void *mmap_ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    printf("  Region        Variable          Address\n");
    printf("  ────────────  ────────────────  ──────────────────\n");
    printf("  .text         demo_live_addr..  %p\n", (void*)demo_live_addresses);
    printf("  .text         demo_memory_map   %p\n", (void*)demo_memory_map);
    printf("  .rodata       ro_msg            %p\n", (void*)ro_msg);
    printf("  .rodata       ro_array          %p\n", (void*)ro_array);
    printf("  .data         rw_global         %p  (val=%d)\n", (void*)&rw_global, rw_global);
    printf("  .data         rw_static         %p  (val=%d)\n", (void*)&rw_static, rw_static);
    printf("  .bss          bss_global        %p  (val=%d)\n", (void*)&bss_global, bss_global);
    printf("  .bss          bss_static        %p  (val=%d)\n", (void*)&bss_static, bss_static);
    printf("  .bss          bss_buffer        %p\n", (void*)bss_buffer);
    printf("  Heap          heap_ptr          %p  (val=%d)\n", (void*)heap_ptr, heap_ptr[0]);
    printf("  mmap          mmap_ptr          %p\n", mmap_ptr);
    printf("  Stack         stack_var         %p  (val=%d)\n", (void*)&stack_var, stack_var);
    printf("  Stack         stack_buf         %p  (\"%s\")\n", (void*)stack_buf, stack_buf);
    printf("\n");

    printf("  Notice the address ordering (typical):\n");
    printf("    .text < .rodata < .data < .bss < heap < ... < mmap < ... < stack\n\n");

    free(heap_ptr);
    if (mmap_ptr != MAP_FAILED) munmap(mmap_ptr, 4096);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — The Heap (malloc / brk / mmap)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_heap(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — The Heap                                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  malloc() obtains memory from the heap.\n");
    printf("  Under the hood, glibc's malloc uses:\n\n");

    printf("  ┌────────────────┬──────────────────────────────────────┐\n");
    printf("  │ Method         │ When used                            │\n");
    printf("  ├────────────────┼──────────────────────────────────────┤\n");
    printf("  │ brk() / sbrk() │ Small allocations (< 128 KB)        │\n");
    printf("  │                │ Extends the data segment upward.     │\n");
    printf("  ├────────────────┼──────────────────────────────────────┤\n");
    printf("  │ mmap()         │ Large allocations (>= 128 KB)        │\n");
    printf("  │                │ Anonymous private mapping.            │\n");
    printf("  └────────────────┴──────────────────────────────────────┘\n\n");

    /* Demonstrate small vs large allocation */
    void *small = malloc(1024);        /* likely via brk */
    void *large = malloc(256 * 1024);  /* likely via mmap */

    printf("  Small allocation (1 KB):   %p  (via brk, near heap)\n", small);
    printf("  Large allocation (256 KB): %p  (via mmap, near libs)\n\n", large);

    printf("  The brk pointer:\n");
    void *current_brk = sbrk(0);
    printf("    Current brk = %p\n", current_brk);
    printf("    (This is the top of the heap data segment.)\n\n");

    free(small);
    free(large);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — The Stack
 * ════════════════════════════════════════════════════════════════════ */

/* Recursive function to demonstrate stack growth */
static void stack_depth(int depth, void *initial_sp)
{
    int local_var = depth;
    if (depth == 0) {
        printf("    Depth 0:  &local_var = %p\n", (void*)&local_var);
        long diff = (char*)initial_sp - (char*)&local_var;
        printf("    Stack consumed: ~%ld bytes over %d frames\n\n",
               diff, 10);
        return;
    }
    if (depth == 10)
        printf("    Depth 10: &local_var = %p (start)\n", (void*)&local_var);
    stack_depth(depth - 1, initial_sp);
    (void)local_var;
}

static void demo_stack(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — The Stack                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  The stack grows downward on x86-64.\n");
    printf("  Default size: 8 MB (ulimit -s → 8192 KB).\n\n");

    printf("  Each function call pushes a stack frame:\n");
    printf("    ┌─────────────────────┐\n");
    printf("    │ Return address       │\n");
    printf("    │ Saved rbp            │\n");
    printf("    │ Local variables       │\n");
    printf("    │ Saved registers       │\n");
    printf("    └─────────────────────┘\n\n");

    printf("  Demonstrating stack growth with recursion:\n");
    int anchor;
    stack_depth(10, &anchor);

    printf("  Stack overflow:\n");
    printf("    If recursion is too deep, the stack hits the guard page.\n");
    printf("    Kernel sends SIGSEGV → \"Segmentation fault (core dumped)\".\n\n");

    printf("  Adjusting stack size:\n");
    printf("    ulimit -s 16384       (set to 16 MB)\n");
    printf("    ulimit -s unlimited   (no limit — use cautiously)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — /proc/self/maps
 * ════════════════════════════════════════════════════════════════════ */
static void demo_proc_maps(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — /proc/self/maps                            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  /proc/self/maps shows all memory mappings of the process.\n\n");

    printf("  Format: start-end perms offset dev inode pathname\n\n");
    printf("  Typical entries:\n");
    printf("    55a3b0400000-55a3b0401000 r--p  ...  /path/to/program\n");
    printf("    55a3b0401000-55a3b0402000 r-xp  ...  /path/to/program    [.text]\n");
    printf("    55a3b0402000-55a3b0403000 r--p  ...  /path/to/program    [.rodata]\n");
    printf("    55a3b0403000-55a3b0404000 rw-p  ...  /path/to/program    [.data+.bss]\n");
    printf("    55a3b1200000-55a3b1221000 rw-p  ...  [heap]\n");
    printf("    7f8a90000000-7f8a90200000 r-xp  ...  /lib/x86_64.../libc.so.6\n");
    printf("    7ffc3a800000-7ffc3a821000 rw-p  ...  [stack]\n");
    printf("    7ffc3a9fe000-7ffc3aa00000 r-xp  ...  [vdso]\n\n");

    printf("  Permissions:\n");
    printf("    r = read, w = write, x = execute, p = private, s = shared\n\n");

    /* Print first few lines of /proc/self/maps */
    printf("  First 10 lines of THIS process's /proc/self/maps:\n");
    printf("  ─────────────────────────────────────────────────────\n");
    FILE *maps = fopen("/proc/self/maps", "r");
    if (maps) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), maps) && count < 10) {
            printf("  %s", line);
            count++;
        }
        fclose(maps);
    } else {
        printf("  (Could not open /proc/self/maps)\n");
    }
    printf("  ...\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — ASLR (Address Space Layout Randomisation)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_aslr(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — ASLR                                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  ASLR randomises the base addresses of:\n");
    printf("    - Stack\n");
    printf("    - mmap region (shared libraries)\n");
    printf("    - Heap\n");
    printf("    - PIE executable base (if compiled with -fPIE)\n\n");

    printf("  Run this program twice and compare addresses:\n");
    printf("    demo()  = %p\n", (void*)demo_aslr);
    printf("    (Address changes each run if PIE is enabled.)\n\n");

    printf("  Check ASLR status:\n");
    printf("    cat /proc/sys/kernel/randomize_va_space\n");
    printf("    0 = off,  1 = stack/mmap,  2 = full (default)\n\n");

    printf("  Purpose: makes exploits harder (attacker can't predict\n");
    printf("  addresses for return-oriented programming, etc.).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 29 — Process Memory Layout                         ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_memory_map();
    demo_live_addresses();
    demo_heap();
    demo_stack();
    demo_proc_maps();
    demo_aslr();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 29 — Process Memory Layout\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
