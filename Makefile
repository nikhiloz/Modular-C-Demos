CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2 -g -Wno-unused-parameter -Wno-unused-variable
PTHREAD := -pthread
INCDIR := include
BINDIR := bin

.PHONY: all clean test help directories

all: directories $(BINDIR)/01_data_types $(BINDIR)/02_operators $(BINDIR)/03_control_flow $(BINDIR)/04_functions $(BINDIR)/05_arrays $(BINDIR)/06_pointers $(BINDIR)/07_strings $(BINDIR)/08_structures $(BINDIR)/09_memory $(BINDIR)/10_file_io $(BINDIR)/11_preprocessor $(BINDIR)/12_bitwise $(BINDIR)/13_advanced $(BINDIR)/14_concurrency $(BINDIR)/15_system $(BINDIR)/c_demos
	@echo "Build complete! Demos are in $(BINDIR)/"
	@ls -la $(BINDIR)/

directories:
	@mkdir -p $(BINDIR)

$(BINDIR)/c_demos: src/main.c
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

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

test: all
	@echo "Running all demos..."
	@for demo in $(BINDIR)/*; do echo "--- $$demo ---"; $$demo 2>&1 | head -50 || true; done

clean:
	rm -rf $(BINDIR)

help:
	@echo "make       - Build all demos"
	@echo "make test  - Build and run all demos"
	@echo "make clean - Clean build files"
