CC = g++

CFLAGS = -Wall -pthread

default: ex

ex: ex.o GPIO.o PWM.o util.o
	$(CC) $(CFLAGS) -o ex ex.o PWM.o GPIO.o

ex.o: ex.cpp
	$(CC) $(CFLAGS) -c ex.cpp


PWM.o: PWM.cpp PWM.h util.o
	$(CC) $(CFLAGS) -c -o PWM.cpp util.o
	
GPIO.o: GPIO.cpp GPIO.h
	$(CC) $(CFLAGS) -c GPIO.cpp 

util.o: util.cpp util.h
	$(CC) $(CFLAGS) -c -o util.cpp

clean:
	$(RM) ex *.o *~
