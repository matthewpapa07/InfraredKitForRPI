CC = gcc
LIBS = -lrt -lwiringPi
DEPS = IrRepeat.h
CFLAGS = -Wall
OBJ = IrRepeat.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

IrRepeat: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
