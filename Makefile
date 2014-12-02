CFLAGS = -O2 -Wall -std=c++11
CC=g++-4.9

all : simtest simDemo1 simDemo2

simtest: simulator.o simtest.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 simulator.o simtest.o -o simtest
	
simDemo1: simulator.o simDemo1.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 simulator.o simDemo1.o -o simDemo1
		
simDemo2: simulator.o simDemo2.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 simulator.o simDemo2.o -o simDemo2
	
simulator.o: simulator.cpp simulator.h
		$(CC) $(CFLAGS) -o $@ -c $<

simtest.o: simtest.cpp simulator.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
simDemo1.o: simDemo1.cpp simulator.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
simDemo2.o: simDemo2.cpp simulator.h
		$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o
	
cleanall: clean
	rm -f simtest simDemo1 simDemo2
	
	
