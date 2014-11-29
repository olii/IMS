CFLAGS = -g3 -Wall -std=c++11
CC=g++-4.9

all : simtest

simtest: core.o simtest.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 core.o simtest.o -o simtest
	

core.o: core.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<

simtest.o: simtest.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
	
clean:
	rm *.o
	
