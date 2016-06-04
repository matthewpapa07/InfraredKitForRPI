import RPi.GPIO as GPIO
import time

#GPIO.cleanup()
GPIO.setmode(GPIO.BCM)

GPIO.setup(4, GPIO.OUT, initial=GPIO.HIGH)

print("Start")
for i in range (0,9000):
	GPIO.output(4, GPIO.HIGH)
	time.sleep(0.001)
	GPIO.output(4, GPIO.LOW)
	time.sleep(0.001)
  
GPIO.cleanup()
print("End")
