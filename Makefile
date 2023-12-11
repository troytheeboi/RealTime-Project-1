# Makefile for your_program_name

CC = gcc
CFLAGS =  -Wall -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lm -g

# List of source files
SRCS = main.c fileReaders.c openGLfunctions.c cashierSm_Sem.c cashierProc.c randomNumGen.c customer.c watcher.c

# List of object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = exec

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) 

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
