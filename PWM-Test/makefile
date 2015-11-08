CC = g++

CFLAGS = -Wall -pthread

default: test

ex: ex.o GPIO.o PWM.o
	$(CC) $(CFLAGS) -o test test.o comm.o

ex.o: ex.cpp
	$(CC) $(CFLAGS) -c ex.cpp

PWM.o: PWM.cpp PWM.h
	$(CC) $(CFLAGS) -c PWM.cpp 
	
GPIO.o: GPIO.cpp GPIO.h
	$(CC) $(CFLAGS) -c GPIO.cpp 

clean:
	$(RM) count *.o *~