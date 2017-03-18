CC = gcc
LIBS = -lrt -lwiringPi
DEPS = IrHelpers.h IrCommon.h
CFLAGS = -Wall
program_C_SRCS := $(wildcard *.c)
program_C_OBJS := ${program_C_SRCS:.c=.o}

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

IrKit: $(program_C_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
