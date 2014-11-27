CFLAGS = -g3 -Wall -std=c++11
CC=g++

all : simtest

simtest: core.o simtest.o
        g++ -Wall -g3 -std=c++11 core.o simtest.o -o simtest
	

core.o: core.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<

simtest.o: simtest.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
	
clean:
	rm *.o
	
