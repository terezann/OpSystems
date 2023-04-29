TARGETS = ask

# Targets to build when running `make`
all: $(TARGETS)

# Phony targets. Executed always.
.PHONY: all clean

# Targets to build `project` binary
ask: ask.o
	gcc ask.o -o ask3
ask.o: ask.c
	gcc ask.c -c -o ask.o

# Run `make clean` to delete binaries and object files.
clean:
	rm -rf $(TARGETS) *.o

