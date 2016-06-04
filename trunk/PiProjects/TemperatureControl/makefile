CC = gcc
LIBS = -lrt -lwiringPi
DEPS = TemperatureControl.h
CFLAGS = -Wall
OBJ = TemperatureControl.o TemperatureSensor.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

IrRepeat: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
