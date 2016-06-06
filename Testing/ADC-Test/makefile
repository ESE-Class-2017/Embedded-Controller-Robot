cc = g++

CFLAGS = -Wall -std=c++11 

default: test

test: test.o
	$(cc) $(CFLAGS) -o test test.o

test.o: test.cpp
	$(cc) $(CFLAGS) -c test.cpp
