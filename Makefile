CFLAGS = -g -Wall -std=c++11
CC=g++


all: core.o simtest.o
	g++ -g -Wall -O2 -std=c++11 core.o simtest.o -o simtest
	

core.o: core.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<

simtest.o: simtest.cpp
		$(CC) $(CFLAGS) -o $@ -c $<
		
	
clean:
	rm *.o
	
