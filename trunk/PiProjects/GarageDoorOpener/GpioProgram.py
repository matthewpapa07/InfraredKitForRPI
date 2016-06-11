import RPi.GPIO as GPIO
import time
import socket               # Import socket module
s = socket.socket()         # Create a socket object
host = socket.gethostname() # Get local machine name
port = 1337                 # Reserve a port for your service.
s.bind((host, port))        # Listen on this port
s.listen(1)

# GPIO Setup Stuff
GPIO.cleanup()
GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.OUT, initial=GPIO.LOW)

print 'Start Garage Door Opener'
print 'Listening at ' + host + ':' + str(port)

while True:
	c, addr = s.accept() # This is a blocking method
	print 'Got connection from ', addr
	data = c.recv(12)
	if not data: break
	c.sendall('Succesful request!')
	c.close()
GPIO.cleanup()
print 'End'
