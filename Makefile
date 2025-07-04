CC = gcc

CFLAGS = -Wall -g

TARGET = Pharmacy_Management

SRCS = main.c mainFn.c helpingFn.c supportiveFn.c

OBJS = $(SRCS: .c = .o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
