CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2 -g -Wno-unused-parameter -Wno-unused-variable
PTHREAD := -pthread
INCDIR := include
BINDIR := bin

.PHONY: all clean test help directories

# ── Part I: C Fundamentals (ch01-15) ─────────────────────────────
PART1 := $(BINDIR)/01_data_types $(BINDIR)/02_operators $(BINDIR)/03_control_flow \
         $(BINDIR)/04_functions $(BINDIR)/05_arrays $(BINDIR)/06_pointers \
         $(BINDIR)/07_strings $(BINDIR)/08_structures $(BINDIR)/09_memory \
         $(BINDIR)/10_file_io $(BINDIR)/11_preprocessor $(BINDIR)/12_bitwise \
         $(BINDIR)/13_advanced $(BINDIR)/14_concurrency $(BINDIR)/15_system

# ── Part II: How the Compiler Works (ch16-25) ───────────────────
PART2 := $(BINDIR)/16_compilation_overview $(BINDIR)/17_preprocessor_deep \
         $(BINDIR)/18_lexical_analysis $(BINDIR)/19_parsing_ast \
         $(BINDIR)/20_semantic_analysis $(BINDIR)/21_intermediate_repr \
         $(BINDIR)/22_optimisation $(BINDIR)/23_code_generation \
         $(BINDIR)/24_assembler_elf $(BINDIR)/25_linker

# ── Part III: Program Loading & Execution (ch26-32) ─────────────
PART3 := $(BINDIR)/26_elf_executable $(BINDIR)/27_kernel_exec \
         $(BINDIR)/28_dynamic_linker $(BINDIR)/29_memory_layout \
         $(BINDIR)/30_crt_startup $(BINDIR)/31_calling_conventions \
         $(BINDIR)/32_termination

# ── Part IV: Practical Depth (ch33-36) ──────────────────────────
PART4 := $(BINDIR)/33_debugging_tools $(BINDIR)/34_libraries \
         $(BINDIR)/35_cross_compilation $(BINDIR)/36_virtual_memory

all: directories $(PART1) $(PART2) $(PART3) $(PART4) $(BINDIR)/c_demos
	@echo "Build complete! Demos are in $(BINDIR)/"
	@ls -la $(BINDIR)/

directories:
	@mkdir -p $(BINDIR)

# ── Master runner ────────────────────────────────────────────────
$(BINDIR)/c_demos: src/main.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# ── Part I targets ───────────────────────────────────────────────
$(BINDIR)/01_data_types: src/01_basics/data_types.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/02_operators: src/02_operators/operators.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/03_control_flow: src/03_control_flow/control_flow.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/04_functions: src/04_functions/functions.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/05_arrays: src/05_arrays/arrays.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/06_pointers: src/06_pointers/pointers.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/07_strings: src/07_strings/strings.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/08_structures: src/08_structures/structures.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/09_memory: src/09_memory/memory.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/10_file_io: src/10_file_io/file_io.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/11_preprocessor: src/11_preprocessor/preprocessor.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/12_bitwise: src/12_bitwise/bitwise.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/13_advanced: src/13_advanced/advanced.c
	$(CC) $(CFLAGS) -std=c11 -I$(INCDIR) $< -o $@

$(BINDIR)/14_concurrency: src/14_concurrency/concurrency.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@ $(PTHREAD)

$(BINDIR)/15_system: src/15_system/system.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# ── Part II targets ──────────────────────────────────────────────
$(BINDIR)/16_compilation_overview: src/16_compilation_overview/compilation_overview.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/17_preprocessor_deep: src/17_preprocessor_deep/preprocessor_deep.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/18_lexical_analysis: src/18_lexical_analysis/lexical_analysis.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/19_parsing_ast: src/19_parsing_ast/parsing_ast.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/20_semantic_analysis: src/20_semantic_analysis/semantic_analysis.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/21_intermediate_repr: src/21_intermediate_repr/intermediate_repr.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/22_optimisation: src/22_optimisation/optimisation.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/23_code_generation: src/23_code_generation/code_generation.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/24_assembler_elf: src/24_assembler_elf/assembler_elf.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/25_linker: src/25_linker/linker.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# ── Part III targets ─────────────────────────────────────────────
$(BINDIR)/26_elf_executable: src/26_elf_executable/elf_executable.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/27_kernel_exec: src/27_kernel_exec/kernel_exec.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/28_dynamic_linker: src/28_dynamic_linker/dynamic_linker.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@ -ldl

$(BINDIR)/29_memory_layout: src/29_memory_layout/memory_layout.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/30_crt_startup: src/30_crt_startup/crt_startup.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/31_calling_conventions: src/31_calling_conventions/calling_conventions.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/32_termination: src/32_termination/termination.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# ── Part IV targets ──────────────────────────────────────────────
$(BINDIR)/33_debugging_tools: src/33_debugging_tools/debugging_tools.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/34_libraries: src/34_libraries/libraries.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/35_cross_compilation: src/35_cross_compilation/cross_compilation.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(BINDIR)/36_virtual_memory: src/36_virtual_memory/virtual_memory.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

# ── Convenience targets ─────────────────────────────────────────
part1: directories $(PART1)
	@echo "Part I built."

part2: directories $(PART2)
	@echo "Part II built."

part3: directories $(PART3)
	@echo "Part III built."

part4: directories $(PART4)
	@echo "Part IV built."

test: all
	@echo "Running all demos..."
	@for demo in $(BINDIR)/*; do echo "--- $$demo ---"; $$demo 2>&1 | head -50 || true; done

clean:
	rm -rf $(BINDIR)

help:
	@echo "make        - Build all demos (Parts I-IV)"
	@echo "make part1  - Build Part I   (C Fundamentals, ch01-15)"
	@echo "make part2  - Build Part II  (Compiler Internals, ch16-25)"
	@echo "make part3  - Build Part III (Program Loading, ch26-32)"
	@echo "make part4  - Build Part IV  (Practical Depth, ch33-36)"
	@echo "make test   - Build and run all demos"
	@echo "make clean  - Clean build files"
