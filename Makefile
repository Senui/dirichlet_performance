# Makefile for compiling C++ source files

# Compiler to use
CC = g++

# Flags to pass to the compiler
CFLAGS = -std=c++17 -O3

LIBS = -lgsl -fopenmp

# Target file
TARGET = dirichlet_performance

# Target file
OMP_TARGET = dirichlet_performance_omp

# Source files
SOURCES = dirichlet_performance.cc

# Object files
OBJECTS = $(SOURCES:.cc=.o)

# Default target
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

# OpenMP target
$(OMP_TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -DFOO_OMP $(OBJECTS) -o $(OMP_TARGET) $(LIBS)

# Rule for creating object files from source files
%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
