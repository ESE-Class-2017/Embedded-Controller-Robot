CC = g++

CFLAGS = -Wall -std=c++11 -pthread

default: test

test: test.o comm.o queue.o
	$(CC) $(CFLAGS) -o test test.o comm.o queue.o

test.o: test.cpp comm.h
	$(CC) $(CFLAGS) -c test.cpp

comm.o: comm.cpp comm.h
	$(CC) $(CFLAGS) -c comm.cpp 
	
queue.o: queue.cpp queue.h
	$(CC) $(CFLAGS) -c queue.cpp

clean:
	$(RM) count *.o *~
