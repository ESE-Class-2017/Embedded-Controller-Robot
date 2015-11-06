CC = g++

CFLAGS = -Wall -std=c++11 -pthread

default: test

test: test.o comm.o
	$(CC) $(CFLAGS) -o test test.o comm.o

test.o: test.cpp comm.h
	$(CC) $(CFLAGS) -c test.cpp

comm.o: comm.cpp comm.h
	$(CC) $(CFLAGS) -c comm.cpp 

clean:
	$(RM) count *.o *~
