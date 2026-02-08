/* ╔══════════════════════════════════════════════════════════════════╗
 * ║  Chapter 24 — Assembler & ELF Object Files                      ║
 * ║  Modular-C-Demos                                                ║
 * ║  Topics: .s→.o, ELF format, sections, symbols, relocations     ║
 * ╚══════════════════════════════════════════════════════════════════╝ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ── Data items placed into various ELF sections ─────────────── */

/* .rodata — read-only data (const globals, string literals) */
const char  ro_string[]   = "Hello from .rodata";
const int   ro_table[4]   = {1, 2, 3, 4};

/* .data — initialised read-write data */
int         rw_int        = 42;
static int  rw_static     = 99;  /* LOCAL symbol in .data */
char        rw_buf[8]     = "initial";

/* .bss — uninitialised (zero-initialised) data */
int         bss_int;             /* GLOBAL, zero-filled */
static int  bss_static;         /* LOCAL, zero-filled */
char        bss_buf[256];       /* zero-filled at load */

/* .text — code */
static int helper_func(int x) { return x * 2; }  /* LOCAL in .text */
int         public_func(int x) { return x + 1; }  /* GLOBAL in .text */

/* ════════════════════════════════════════════════════════════════════
 *  Section 1 — What the Assembler Does
 * ════════════════════════════════════════════════════════════════════ */
static void demo_assembler(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 1 — What the Assembler Does                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The assembler (as / gas) converts assembly (.s) into an\n");
    printf("object file (.o) in ELF format (on Linux).\n\n");

    printf("  .s file (human-readable)    →    .o file (binary ELF)\n\n");

    printf("Tasks of the assembler:\n");
    printf("  1. Translate mnemonics to machine opcodes.\n");
    printf("     mov eax, edi  →  89 f8  (hex bytes)\n");
    printf("  2. Resolve local labels to offsets.\n");
    printf("  3. Create relocation entries for external symbols.\n");
    printf("  4. Organise code/data into ELF sections.\n");
    printf("  5. Build the symbol table (.symtab).\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Two-pass assembly:                                     ║\n");
    printf("║    Pass 1: Collect all label addresses (symbol table).  ║\n");
    printf("║    Pass 2: Emit machine code, patch known addresses.   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 2 — ELF Object File Format
 * ════════════════════════════════════════════════════════════════════ */
static void demo_elf_format(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 2 — ELF Object File Format                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("ELF (Executable and Linkable Format) is the standard on Linux.\n\n");

    printf("  Object file (.o) layout:\n");
    printf("  ┌──────────────────────────────┐\n");
    printf("  │ ELF Header (64 bytes)        │  Magic: 7f 45 4c 46\n");
    printf("  │   - Class (32/64-bit)        │\n");
    printf("  │   - Endianness               │\n");
    printf("  │   - Machine type             │\n");
    printf("  │   - Entry point (0 for .o)   │\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .text    (machine code)      │\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .data    (initialised vars)  │\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .bss     (zero-init vars)    │  Takes no space in file\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .rodata  (read-only data)    │\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .symtab  (symbol table)      │\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .strtab  (string table)      │  Symbol names\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ .rela.text (relocations)     │  Fixup instructions\n");
    printf("  ├──────────────────────────────┤\n");
    printf("  │ Section Header Table         │  Describes all sections\n");
    printf("  └──────────────────────────────┘\n\n");

    printf("  ELF magic bytes: 0x7f 'E' 'L' 'F'\n");
    printf("  Verify: hexdump -C assembler_elf.o | head -4\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 3 — Key ELF Sections
 * ════════════════════════════════════════════════════════════════════ */
static void demo_elf_sections(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 3 — Key ELF Sections                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("Each section holds a specific kind of data:\n\n");

    printf("  ┌─────────────┬──────────────────────────────────────────┐\n");
    printf("  │ Section     │ Contents                                 │\n");
    printf("  ├─────────────┼──────────────────────────────────────────┤\n");
    printf("  │ .text       │ Machine code (functions)                 │\n");
    printf("  │ .data       │ Initialised global/static variables      │\n");
    printf("  │ .bss        │ Uninitialised globals (zero-filled)      │\n");
    printf("  │ .rodata     │ Read-only data (const, string literals)  │\n");
    printf("  │ .symtab     │ Symbol table (names + types + sections)  │\n");
    printf("  │ .strtab     │ String table (null-terminated names)     │\n");
    printf("  │ .rela.text  │ Relocation entries for .text             │\n");
    printf("  │ .comment    │ Compiler version string                  │\n");
    printf("  │ .note.*     │ Build metadata (GNU build-id)            │\n");
    printf("  └─────────────┴──────────────────────────────────────────┘\n\n");

    printf("Demo — which section does each go into?\n\n");

    printf("  const char ro_string[] = \"Hello\";    → .rodata\n");
    printf("  const int  ro_table[4] = {1,2,3,4};  → .rodata\n");
    printf("  int        rw_int = 42;              → .data\n");
    printf("  static int rw_static = 99;           → .data  (LOCAL sym)\n");
    printf("  int        bss_int;                  → .bss   (GLOBAL)\n");
    printf("  static int bss_static;               → .bss   (LOCAL)\n");
    printf("  char       bss_buf[256];             → .bss   (256 bytes)\n");
    printf("  int public_func(int x) {...}         → .text  (GLOBAL)\n");
    printf("  static int helper_func(int x) {...}  → .text  (LOCAL)\n\n");

    /* Use the variables so they aren't optimised away */
    printf("  Values: ro_string=\"%s\", rw_int=%d, rw_static=%d\n",
           ro_string, rw_int, rw_static);
    printf("          bss_int=%d, bss_static=%d\n", bss_int, bss_static);
    printf("          helper_func(5)=%d, public_func(5)=%d\n\n",
           helper_func(5), public_func(5));
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 4 — Symbol Table
 * ════════════════════════════════════════════════════════════════════ */
static void demo_symbol_table(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 4 — Symbol Table                               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("The symbol table (.symtab) lists every named entity:\n\n");

    printf("  Symbol types:\n");
    printf("  ┌───────────────┬──────────────────────────────────────┐\n");
    printf("  │ Type          │ Meaning                              │\n");
    printf("  ├───────────────┼──────────────────────────────────────┤\n");
    printf("  │ STB_LOCAL     │ Visible only in this file (static)   │\n");
    printf("  │ STB_GLOBAL    │ Visible to all files                 │\n");
    printf("  │ STB_WEAK      │ Global, but can be overridden        │\n");
    printf("  ├───────────────┼──────────────────────────────────────┤\n");
    printf("  │ STT_FUNC      │ Function (code)                     │\n");
    printf("  │ STT_OBJECT    │ Data object (variable)              │\n");
    printf("  │ STT_NOTYPE    │ Type not specified                  │\n");
    printf("  │ SHN_UNDEF     │ Undefined (need linker to resolve)  │\n");
    printf("  └───────────────┴──────────────────────────────────────┘\n\n");

    printf("  Expected nm output for this file:\n");
    printf("    0000...  T main              (GLOBAL, .text)\n");
    printf("    0000...  T public_func       (GLOBAL, .text)\n");
    printf("    0000...  t helper_func       (LOCAL,  .text)\n");
    printf("    0000...  D rw_int            (GLOBAL, .data)\n");
    printf("    0000...  d rw_static         (LOCAL,  .data)\n");
    printf("    0000...  R ro_string         (GLOBAL, .rodata)\n");
    printf("    0000...  B bss_int           (GLOBAL, .bss)\n");
    printf("    0000...  b bss_static        (LOCAL,  .bss)\n");
    printf("             U printf            (UNDEFINED — external)\n\n");

    printf("  nm symbol codes:\n");
    printf("    T/t = .text,  D/d = .data,  B/b = .bss\n");
    printf("    R/r = .rodata,  U = undefined (external)\n");
    printf("    Uppercase = GLOBAL,  lowercase = LOCAL\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 5 — Relocations
 * ════════════════════════════════════════════════════════════════════ */
static void demo_relocations(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 5 — Relocations                                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("When the assembler encounters a reference to an external\n");
    printf("symbol (like printf), it can't fill in the address yet.\n");
    printf("Instead, it creates a relocation entry.\n\n");

    printf("  A relocation entry says:\n");
    printf("    'At offset X in section .text, patch in the address\n");
    printf("     of symbol Y, using relocation type Z.'\n\n");

    printf("  Common x86-64 relocation types:\n");
    printf("  ┌──────────────────────┬──────────────────────────────┐\n");
    printf("  │ Type                 │ Meaning                      │\n");
    printf("  ├──────────────────────┼──────────────────────────────┤\n");
    printf("  │ R_X86_64_PC32        │ 32-bit PC-relative           │\n");
    printf("  │ R_X86_64_PLT32       │ 32-bit PLT-relative call     │\n");
    printf("  │ R_X86_64_GOTPCRELX   │ GOT entry, PC-relative       │\n");
    printf("  │ R_X86_64_64          │ Absolute 64-bit address      │\n");
    printf("  └──────────────────────┴──────────────────────────────┘\n\n");

    printf("  Example relocation (from objdump -r):\n");
    printf("    OFFSET           TYPE               VALUE\n");
    printf("    000000000042     R_X86_64_PLT32     printf-0x4\n\n");

    printf("  The linker resolves these in the final step.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 6 — Examining Object Files
 * ════════════════════════════════════════════════════════════════════ */
static void demo_tools(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 6 — Examining Object Files                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Compile to object file:                                ║\n");
    printf("║  gcc -c assembler_elf.c -o assembler_elf.o              ║\n");
    printf("║                                                         ║\n");
    printf("║  View ELF header:                                       ║\n");
    printf("║  readelf -h assembler_elf.o                             ║\n");
    printf("║                                                         ║\n");
    printf("║  List sections:                                         ║\n");
    printf("║  readelf -S assembler_elf.o                             ║\n");
    printf("║                                                         ║\n");
    printf("║  List symbols:                                          ║\n");
    printf("║  readelf -s assembler_elf.o                             ║\n");
    printf("║  nm assembler_elf.o                                     ║\n");
    printf("║                                                         ║\n");
    printf("║  View relocations:                                      ║\n");
    printf("║  readelf -r assembler_elf.o                             ║\n");
    printf("║  objdump -r assembler_elf.o                             ║\n");
    printf("║                                                         ║\n");
    printf("║  Disassemble:                                           ║\n");
    printf("║  objdump -d -M intel assembler_elf.o                    ║\n");
    printf("║                                                         ║\n");
    printf("║  Hex dump of .rodata:                                   ║\n");
    printf("║  objdump -s -j .rodata assembler_elf.o                  ║\n");
    printf("║                                                         ║\n");
    printf("║  View raw hex:                                          ║\n");
    printf("║  hexdump -C assembler_elf.o | head -20                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Section 7 — Section Flags & Attributes
 * ════════════════════════════════════════════════════════════════════ */
static void demo_section_flags(void)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Section 7 — Section Flags & Attributes                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("  Each section has flags controlling memory protection:\n\n");
    printf("  ┌───────────┬───────┬───────┬─────────┬───────────────┐\n");
    printf("  │ Section   │ Read  │ Write │ Execute │ In file?      │\n");
    printf("  ├───────────┼───────┼───────┼─────────┼───────────────┤\n");
    printf("  │ .text     │  Yes  │  No   │  Yes    │ Yes           │\n");
    printf("  │ .rodata   │  Yes  │  No   │  No     │ Yes           │\n");
    printf("  │ .data     │  Yes  │  Yes  │  No     │ Yes           │\n");
    printf("  │ .bss      │  Yes  │  Yes  │  No     │ No (zerofill)  │\n");
    printf("  └───────────┴───────┴───────┴─────────┴───────────────┘\n\n");

    printf("  .bss occupies no space in the object file!\n");
    printf("  It only records the required size. The OS zero-fills it\n");
    printf("  when loading the program into memory.\n\n");

    printf("  This is why 'static int arr[1000000];' doesn't make\n");
    printf("  the executable file 4 MB larger.\n\n");
}

/* ════════════════════════════════════════════════════════════════════
 *  Main
 * ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Chapter 24 — Assembler & ELF Object Files                  ║\n");
    printf("║  Modular-C-Demos                                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    demo_assembler();
    demo_elf_format();
    demo_elf_sections();
    demo_symbol_table();
    demo_relocations();
    demo_tools();
    demo_section_flags();

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  End of Chapter 24 — Assembler & ELF Object Files\n");
    printf("════════════════════════════════════════════════════════════════\n\n");

    return 0;
}
