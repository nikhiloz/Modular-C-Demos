/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 26 — ELF Executable Layout                             ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: ELF header, program headers, segments, entry point     ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — From Object Files to Executables
 * ════════════════════════════════════════════════════════════════════ */
static void demo_obj_to_exe(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — From Object Files to Executables           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("After the linker merges all .o files, the result is an\n");
    printf("ELF executable.\n\n");

    printf("  Object file (.o):          Executable (a.out):\n");
    printf("  ┌────────────────┐         ┌────────────────────┐\n");
    printf("  │ Section headers │         │ Program headers    │ ← NEW!\n");
    printf("  │ .text           │         │ Segment table      │ ← NEW!\n");
    printf("  │ .data           │   ──►   │ .text (merged)     │\n");
    printf("  │ .bss            │         │ .data (merged)     │\n");
    printf("  │ .symtab         │         │ .bss  (merged)     │\n");
    printf("  │ .rela.text      │         │ .dynamic           │ ← NEW!\n");
    printf("  └────────────────┘         │ .interp            │ ← NEW!\n");
    printf("                              │ Section headers    │\n");
    printf("                              └────────────────────┘\n\n");

    printf("Key differences:\n");
    printf("  - Executable has program headers (segments for loading).\n");
    printf("  - All relocations are resolved (no .rela.text).\n");
    printf("  - Entry point is set (address of _start).\n");
    printf("  - .interp section names the dynamic linker.\n");
    printf("  - .dynamic section lists shared library dependencies.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — ELF Header
 * ════════════════════════════════════════════════════════════════════ */
static void demo_elf_header(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — ELF Header                                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The first 64 bytes of every ELF file:\n\n");

    printf("  Offset  Field                Value (typical x86-64)\n");
    printf("  ──────  ───────────────────  ─────────────────────────\n");
    printf("  0x00    e_ident[0..3]        7f 45 4c 46 ('.ELF')\n");
    printf("  0x04    Class                2 (64-bit)\n");
    printf("  0x05    Data                 1 (little-endian)\n");
    printf("  0x06    Version              1 (ELF v1)\n");
    printf("  0x07    OS/ABI               0 (UNIX System V)\n");
    printf("  0x10    e_type               2 (ET_EXEC / 3 = ET_DYN)\n");
    printf("  0x12    e_machine            0x3E (EM_X86_64)\n");
    printf("  0x18    e_entry              Entry point address\n");
    printf("  0x20    e_phoff              Program header table offset\n");
    printf("  0x28    e_shoff              Section header table offset\n");
    printf("  0x36    e_phentsize          Size of program header entry\n");
    printf("  0x38    e_phnum              Number of program headers\n\n");

    printf("  e_type values:\n");
    printf("    ET_REL  (1) = Relocatable (.o)\n");
    printf("    ET_EXEC (2) = Executable (fixed address)\n");
    printf("    ET_DYN  (3) = Shared object / PIE executable\n");
    printf("    ET_CORE (4) = Core dump\n\n");

    printf("  Modern Linux creates PIE (Position Independent Executable)\n");
    printf("  by default: e_type = ET_DYN, loaded at random address (ASLR).\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Program Headers (Segments)
 * ════════════════════════════════════════════════════════════════════ */
static void demo_program_headers(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Program Headers (Segments)                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Program headers tell the OS how to load the file into memory.\n");
    printf("Each program header describes a SEGMENT.\n\n");

    printf("  Sections vs Segments:\n");
    printf("    Sections = linker's view (.text, .data, etc.)\n");
    printf("    Segments = loader's view (contiguous memory blocks)\n\n");

    printf("  Multiple sections can be grouped into one segment:\n");
    printf("    LOAD segment (RX): .text + .rodata\n");
    printf("    LOAD segment (RW): .data + .bss\n\n");

    printf("  Typical program headers:\n");
    printf("  ┌───────────┬──────────────────────────────────────────┐\n");
    printf("  │ Type      │ Purpose                                  │\n");
    printf("  ├───────────┼──────────────────────────────────────────┤\n");
    printf("  │ PHDR      │ Program header table itself              │\n");
    printf("  │ INTERP    │ Path to dynamic linker                   │\n");
    printf("  │ LOAD      │ Loadable segment (code or data)          │\n");
    printf("  │ DYNAMIC   │ Dynamic linking info (.dynamic section)  │\n");
    printf("  │ NOTE      │ Build info (build-id)                    │\n");
    printf("  │ GNU_STACK │ Stack permissions (NX = no execute)      │\n");
    printf("  │ GNU_RELRO │ Read-only after relocation               │\n");
    printf("  └───────────┴──────────────────────────────────────────┘\n\n");

    printf("  Each LOAD segment specifies:\n");
    printf("    - File offset and size (bytes in file)\n");
    printf("    - Virtual address and memory size\n");
    printf("    - Permissions: R (read), W (write), X (execute)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Memory Layout of an ELF Executable
 * ════════════════════════════════════════════════════════════════════ */
static void demo_memory_layout(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Memory Layout of the Loaded Executable     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Virtual Address Space (x86-64 Linux):\n\n");
    printf("       0xFFFFFFFF'FFFFFFFF  ┌───────────────────────┐\n");
    printf("                            │  Kernel space          │\n");
    printf("       0x00007FFF'FFFFFFFF  ├───────────────────────┤\n");
    printf("                            │  Stack  ↓              │ grows down\n");
    printf("                            │         ...            │\n");
    printf("                            │  mmap region           │ shared libs\n");
    printf("                            │         ...            │\n");
    printf("                            │  Heap   ↑              │ grows up\n");
    printf("                            ├───────────────────────┤\n");
    printf("                            │  .bss                  │ RW-\n");
    printf("                            │  .data                 │ RW-\n");
    printf("                            ├───────────────────────┤\n");
    printf("                            │  .rodata               │ R--\n");
    printf("                            │  .text                 │ R-X\n");
    printf("                            ├───────────────────────┤\n");
    printf("                            │  ELF headers           │\n");
    printf("       0x00000000'00400000  └───────────────────────┘\n");
    printf("       (or random with PIE)\n\n");

    /* Print actual addresses from this running process */
    printf("  Live addresses from this process:\n");
    printf("    demo_memory_layout = %p  (.text)\n",
           (void*)demo_memory_layout);
    printf("    ro_string          = %p  (.rodata — if const)\n",
           (void*)"Hello from .rodata");
    printf("    global_counter  could be at an address in .data\n");
    printf("    (Use /proc/self/maps to see the full memory map.)\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — The .interp Section
 * ════════════════════════════════════════════════════════════════════ */
static void demo_interp(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — The .interp Section                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("For dynamically linked executables, .interp contains the\n");
    printf("path to the dynamic linker / runtime loader.\n\n");

    printf("  On x86-64 Linux:\n");
    printf("    /lib64/ld-linux-x86-64.so.2\n\n");

    printf("  On ARM (32-bit):\n");
    printf("    /lib/ld-linux-armhf.so.3\n\n");

    printf("  On AArch64:\n");
    printf("    /lib/ld-linux-aarch64.so.1\n\n");

    printf("  View it: readelf -p .interp ./elf_executable\n\n");

    printf("  When the kernel sees .interp, it loads the dynamic linker\n");
    printf("  INSTEAD of jumping directly to the program. The dynamic\n");
    printf("  linker then loads shared libraries and finally transfers\n");
    printf("  control to the program's entry point.\n\n");

    printf("  Static executables have NO .interp section.\n");
    printf("  The kernel jumps directly to the entry point.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Examining Executables
 * ════════════════════════════════════════════════════════════════════ */
static void demo_examine_exe(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Examining Executables                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  View ELF header:                                       ║\n");
    printf("║  readelf -h ./elf_executable                            ║\n");
    printf("║                                                         ║\n");
    printf("║  View program headers (segments):                       ║\n");
    printf("║  readelf -l ./elf_executable                            ║\n");
    printf("║                                                         ║\n");
    printf("║  View section headers:                                  ║\n");
    printf("║  readelf -S ./elf_executable                            ║\n");
    printf("║                                                         ║\n");
    printf("║  View dynamic section:                                  ║\n");
    printf("║  readelf -d ./elf_executable                            ║\n");
    printf("║                                                         ║\n");
    printf("║  View memory map of running process:                    ║\n");
    printf("║  cat /proc/self/maps                                    ║\n");
    printf("║                                                         ║\n");
    printf("║  Compare .o vs executable:                              ║\n");
    printf("║  readelf -h file.o  (e_type = ET_REL, entry = 0x0)     ║\n");
    printf("║  readelf -h a.out   (e_type = ET_DYN, entry = 0x...)   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 26 — ELF Executable Layout                         ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_obj_to_exe();
    demo_elf_header();
    demo_program_headers();
    demo_memory_layout();
    demo_interp();
    demo_examine_exe();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 26 — ELF Executable Layout\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
