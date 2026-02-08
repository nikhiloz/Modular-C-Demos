/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 36 — Virtual Memory Deep Dive                          ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: paging, TLB, page faults, COW, mmap, mprotect         ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __linux__
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#endif

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — Virtual vs Physical Addresses
 * ════════════════════════════════════════════════════════════════════ */
static void demo_virtual_vs_physical(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — Virtual vs Physical Addresses              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Every process sees its own virtual address space.\n");
    printf("  The MMU (Memory Management Unit) hardware translates\n");
    printf("  virtual addresses → physical addresses.\n\n");

    printf("  Why virtual memory?\n");
    printf("    1. Isolation — processes cannot see each other's memory\n");
    printf("    2. Abstraction — each process gets a flat address space\n");
    printf("    3. Overcommit — total virtual > physical RAM\n");
    printf("    4. Sharing — shared libs mapped once in physical RAM\n");
    printf("    5. Protection — read/write/execute permissions per page\n\n");

    printf("  Address translation:\n");
    printf("    ┌──────────────────┐    ┌────────┐    ┌─────────────────┐\n");
    printf("    │ CPU: virtual addr ├───>│  MMU   ├───>│ Physical memory │\n");
    printf("    │ 0x7fff4a8b3000   │    │ (TLB + │    │ 0x1a4f3000      │\n");
    printf("    └──────────────────┘    │ page   │    └─────────────────┘\n");
    printf("                            │ table) │\n");
    printf("                            └────────┘\n\n");

    int x = 42;
    printf("  Demo: &x = %p  (virtual — physical addr is hidden)\n\n", (void *)&x);
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — Pages and Page Tables
 * ════════════════════════════════════════════════════════════════════ */
static void demo_pages(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — Pages and Page Tables                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Memory is divided into fixed-size pages (typically 4 KB).\n\n");

    printf("  Virtual address breakdown (x86-64, 4-level paging):\n");
    printf("  ┌──────────────────────────────────────────────────────┐\n");
    printf("  │ 63..48 │ 47..39 │ 38..30 │ 29..21 │ 20..12 │ 11..0 │\n");
    printf("  │ (sign) │  PML4  │  PDPT  │   PD   │   PT   │offset │\n");
    printf("  │ extend │ 9 bits │ 9 bits │ 9 bits │ 9 bits │12 bits│\n");
    printf("  └──────────────────────────────────────────────────────┘\n\n");

    printf("  4 levels of page tables (x86-64):\n");
    printf("    PML4 (Page Map Level 4)    → 512 entries\n");
    printf("    PDPT (Page Dir Ptr Table)  → 512 entries\n");
    printf("    PD   (Page Directory)      → 512 entries\n");
    printf("    PT   (Page Table)          → 512 entries\n\n");

    printf("  Each entry contains:\n");
    printf("    - Physical page frame number\n");
    printf("    - Present bit (is page in RAM?)\n");
    printf("    - Read/Write bit\n");
    printf("    - User/Supervisor bit\n");
    printf("    - Accessed bit (read since last clear)\n");
    printf("    - Dirty bit (written since last clear)\n");
    printf("    - NX (No-Execute) bit\n\n");

    printf("  Page sizes:\n");
    printf("  ┌─────────────────┬──────────┬──────────────────┐\n");
    printf("  │ Size            │ Name     │ Use case         │\n");
    printf("  ├─────────────────┼──────────┼──────────────────┤\n");
    printf("  │ 4 KB            │ Regular  │ Most allocations │\n");
    printf("  │ 2 MB            │ Huge     │ Databases, VMs   │\n");
    printf("  │ 1 GB            │ Gigantic │ HPC, large DBs   │\n");
    printf("  └─────────────────┴──────────┴──────────────────┘\n\n");

#ifdef __linux__
    long page_size = sysconf(_SC_PAGESIZE);
    printf("  This system's page size: %ld bytes (%ld KB)\n\n",
           page_size, page_size / 1024);
#endif
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — TLB (Translation Lookaside Buffer)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_tlb(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — TLB (Translation Lookaside Buffer)         ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  The TLB is a hardware cache for page table entries.\n\n");

    printf("  Without TLB: every memory access requires 4 page table\n");
    printf("  lookups (4 levels) before accessing the actual data.\n\n");

    printf("  With TLB: cached translations skip the page walk.\n\n");

    printf("  Translation flow:\n");
    printf("    CPU generates virtual address\n");
    printf("     │\n");
    printf("     ├─ TLB hit → physical address (fast, ~1 cycle)\n");
    printf("     │\n");
    printf("     └─ TLB miss → page table walk (slow, ~100 cycles)\n");
    printf("                   → result cached in TLB\n");
    printf("                   → if page not present → page fault\n\n");

    printf("  TLB characteristics:\n");
    printf("    - Typically 64-1536 entries\n");
    printf("    - Separate I-TLB (instructions) and D-TLB (data)\n");
    printf("    - L2 TLB (unified, larger)\n");
    printf("    - Flushed on context switch (unless PCID/ASID used)\n\n");

    printf("  TLB shootdown:\n");
    printf("    When a page table entry changes, all CPUs must\n");
    printf("    invalidate their TLB entries for that address.\n");
    printf("    This IPI (Inter-Processor Interrupt) is expensive.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Page Faults
 * ════════════════════════════════════════════════════════════════════ */
static void demo_page_faults(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Page Faults                                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  A page fault occurs when the CPU accesses a virtual\n");
    printf("  address whose page table entry has Present=0.\n\n");

    printf("  Types of page faults:\n");
    printf("  ┌──────────────────┬──────────────────────────────────┐\n");
    printf("  │ Type             │ What happens                     │\n");
    printf("  ├──────────────────┼──────────────────────────────────┤\n");
    printf("  │ Minor (soft)     │ Page in RAM but not mapped       │\n");
    printf("  │                  │ → kernel maps page, resumes      │\n");
    printf("  ├──────────────────┼──────────────────────────────────┤\n");
    printf("  │ Major (hard)     │ Page must be loaded from disk    │\n");
    printf("  │                  │ → kernel reads from swap/file    │\n");
    printf("  ├──────────────────┼──────────────────────────────────┤\n");
    printf("  │ Invalid          │ Address not mapped at all        │\n");
    printf("  │                  │ → SIGSEGV (segfault)             │\n");
    printf("  └──────────────────┴──────────────────────────────────┘\n\n");

    printf("  Demand paging:\n");
    printf("    Pages are NOT allocated physical RAM until first access.\n");
    printf("    malloc(1GB) → no physical RAM used until written.\n\n");

    printf("  Monitor page faults:\n");
    printf("    /usr/bin/time -v ./program\n");
    printf("    → Minor page faults: 1234\n");
    printf("    → Major page faults: 0\n\n");

    printf("  perf stat -e page-faults ./program\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Copy-on-Write (COW)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_cow(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Copy-on-Write (COW)                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  fork() creates a child process — but doesn't copy memory!\n\n");

    printf("  Instead:\n");
    printf("    1. Parent and child share the same physical pages\n");
    printf("    2. All shared pages are marked read-only\n");
    printf("    3. When either process WRITES, a page fault occurs\n");
    printf("    4. Kernel copies that ONE page, gives the writer\n");
    printf("       a private copy, marks it read-write\n\n");

    printf("  COW timeline:\n");
    printf("    fork():\n");
    printf("      Parent pages: [A][B][C][D]  (read-only)\n");
    printf("      Child pages:  [A][B][C][D]  (same physical pages)\n\n");

    printf("    Child writes to page B:\n");
    printf("      Parent pages: [A][B][C][D]  (B still original)\n");
    printf("      Child pages:  [A][B'][C][D] (B' is a private copy)\n\n");

    printf("  Benefits:\n");
    printf("    - fork() is nearly instant (no memory copied)\n");
    printf("    - fork()+exec() is very cheap (new program replaces\n");
    printf("      all pages, so nothing was ever copied)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — mmap / mprotect Demo
 * ════════════════════════════════════════════════════════════════════ */
#ifdef __linux__
static volatile int got_signal = 0;
static sigjmp_buf jump_buf;

static void segv_handler(int sig)
{
    (void)sig;
    got_signal = 1;
    siglongjmp(jump_buf, 1);
}
#endif

static void demo_mmap(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — mmap and mprotect Demo                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

#ifdef __linux__
    long page_size = sysconf(_SC_PAGESIZE);

    /* Allocate a page with read+write */
    void *page = mmap(NULL, (size_t)page_size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS,
                       -1, 0);
    if (page == MAP_FAILED) {
        perror("mmap");
        return;
    }
    printf("  mmap allocated page at: %p\n", page);

    /* Write to it */
    strcpy((char *)page, "Hello from mmap!");
    printf("  Written: \"%s\"\n", (char *)page);

    /* Make it read-only */
    if (mprotect(page, (size_t)page_size, PROT_READ) == 0) {
        printf("  mprotect set page to READ-ONLY\n");
    }

    /* Try to write — should get SIGSEGV */
    struct sigaction sa, old_sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = segv_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, &old_sa);

    if (sigsetjmp(jump_buf, 1) == 0) {
        /* This write should trigger SIGSEGV */
        ((char *)page)[0] = 'X';
    }

    if (got_signal) {
        printf("  SIGSEGV caught! Write to read-only page blocked.\n");
        got_signal = 0;
    }

    /* Restore and clean up */
    sigaction(SIGSEGV, &old_sa, NULL);
    mprotect(page, (size_t)page_size, PROT_READ | PROT_WRITE);
    munmap(page, (size_t)page_size);
    printf("  Page unmapped.\n\n");

    /* Memory-mapped file I/O */
    printf("  Memory-mapped file I/O:\n");
    printf("    int fd = open(\"data.bin\", O_RDONLY);\n");
    printf("    void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);\n");
    printf("    // Access file contents through 'map' pointer\n");
    printf("    // Reads happen via page faults — no explicit read() calls\n");
    printf("    munmap(map, size);\n\n");

#else
    printf("  (mmap/mprotect demo requires Linux)\n\n");
#endif

    printf("  mmap uses:\n");
    printf("  ┌──────────────────────┬────────────────────────────────┐\n");
    printf("  │ Use case             │ Flags                          │\n");
    printf("  ├──────────────────────┼────────────────────────────────┤\n");
    printf("  │ Anonymous memory     │ MAP_ANONYMOUS | MAP_PRIVATE    │\n");
    printf("  │ File mapping (read)  │ PROT_READ | MAP_PRIVATE        │\n");
    printf("  │ File mapping (write) │ PROT_READ|WRITE | MAP_SHARED   │\n");
    printf("  │ Shared memory (IPC)  │ MAP_SHARED | MAP_ANONYMOUS     │\n");
    printf("  │ JIT code generation  │ PROT_READ|WRITE|EXEC           │\n");
    printf("  │ Guard pages          │ PROT_NONE                      │\n");
    printf("  └──────────────────────┴────────────────────────────────┘\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Swap and Overcommit
 * ════════════════════════════════════════════════════════════════════ */
static void demo_swap(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Swap and Overcommit                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Swap: disk space used as overflow for physical RAM.\n\n");

    printf("  When RAM is full, the kernel evicts (swaps out) pages:\n");
    printf("    1. Find a victim page (LRU-based)\n");
    printf("    2. If dirty, write to swap partition/file\n");
    printf("    3. Mark page table entry as not-present\n");
    printf("    4. Use freed physical frame for new allocation\n");
    printf("    5. When victim page is accessed again → major page fault\n");
    printf("       → read back from swap (thrashing if frequent)\n\n");

    printf("  Overcommit (Linux):\n");
    printf("    vm.overcommit_memory settings:\n");
    printf("    0 = Heuristic (default) — allows moderate overcommit\n");
    printf("    1 = Always allow (dangerous, OOM possible)\n");
    printf("    2 = Never overcommit (commit limit = swap + ratio*RAM)\n\n");

    printf("  OOM Killer:\n");
    printf("    When the system truly runs out of memory:\n");
    printf("    - Kernel selects a process to kill (oom_score)\n");
    printf("    - Sends SIGKILL to free memory\n");
    printf("    - Check: dmesg | grep -i oom\n");
    printf("    - Protect a process: echo -1000 > /proc/PID/oom_score_adj\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 8 — Inspecting Virtual Memory
 * ════════════════════════════════════════════════════════════════════ */
static void demo_inspect(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 8 — Inspecting Virtual Memory                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Useful commands and files:\n\n");

    printf("  ┌─────────────────────────┬────────────────────────────────┐\n");
    printf("  │ Tool / File             │ Purpose                        │\n");
    printf("  ├─────────────────────────┼────────────────────────────────┤\n");
    printf("  │ /proc/PID/maps          │ Virtual memory mappings        │\n");
    printf("  │ /proc/PID/smaps         │ Detailed per-mapping stats     │\n");
    printf("  │ /proc/PID/pagemap       │ Virtual→physical page mapping  │\n");
    printf("  │ /proc/PID/status        │ VmSize, VmRSS, VmSwap         │\n");
    printf("  │ /proc/meminfo           │ System-wide memory info        │\n");
    printf("  │ pmap -x PID             │ Process memory map             │\n");
    printf("  │ vmstat 1                │ VM statistics per second       │\n");
    printf("  │ free -h                 │ System memory summary          │\n");
    printf("  │ valgrind --tool=massif  │ Heap profiling                 │\n");
    printf("  └─────────────────────────┴────────────────────────────────┘\n\n");

#ifdef __linux__
    printf("  Live /proc/self/status snippet:\n");
    FILE *fp = fopen("/proc/self/status", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Vm", 2) == 0 ||
                strncmp(line, "Rss", 3) == 0) {
                printf("    %s", line);
            }
        }
        fclose(fp);
    }
    printf("\n");
#endif
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 36 — Virtual Memory Deep Dive                      ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_virtual_vs_physical();
    demo_pages();
    demo_tlb();
    demo_page_faults();
    demo_cow();
    demo_mmap();
    demo_swap();
    demo_inspect();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 36 — Virtual Memory Deep Dive\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
