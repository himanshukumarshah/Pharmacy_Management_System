# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Output binary
TARGET = Pharmacy_Management

# Source and Object files
SRCS = main.c mainFunctions.c helpingFunctions.c supportiveFunctions.c
OBJS = main.o mainFunctions.o helpingFunctions.o supportiveFunctions.o

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compiling each .c to .o
%.o: %.c structures.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and executable
clean:
	rm -f *.o $(TARGET)
