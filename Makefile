# Makefile for your_program_name

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -lrt -g

# List of source files
SRCS = main.c fileReaders.c

# List of object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = exec

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
