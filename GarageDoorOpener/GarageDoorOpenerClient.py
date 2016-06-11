import RPi.GPIO as GPIO
import time

#GPIO.cleanup()
GPIO.setmode(GPIO.BCM)

GPIO.setup(3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#t2 = time.clock()

print("Start Program" + str(time.clock()))
for i in range (0,25):
#	t1 = time.clock()
	print(time.time())
	GPIO.wait_for_edge(3, GPIO.FALLING)
#	print("To Zero after" + str(t2-t1))
#	t2 = time.clock()
	print(time.time())
	GPIO.wait_for_edge(3, GPIO.RISING)
# 	print("To One after" + str(t1-t2))
GPIO.cleanup()
print("End")
