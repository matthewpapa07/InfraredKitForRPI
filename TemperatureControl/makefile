CC = gcc
LIBS = -lrt -lwiringPi -lm
DEPS = TemperatureControl.h
CFLAGS = -Wall
OBJ = TemperatureControl.o TemperatureSensor.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

TemperatureControl : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
