CFLAGS = -O2 -Wall -std=c++11
CC=g++-4.9

all : simtest simDemo1 simDemo2

simtest: core.o simtest.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 core.o simtest.o -o simtest
	
simDemo1: core.o simDemo1.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 core.o simDemo1.o -o simDemo1
		
simDemo2: core.o simDemo2.o
		$(CC) $(CFLAGS) -Wl,-rpath,/usr/local/lib64 core.o simDemo2.o -o simDemo2
	
core.o: core.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<

simtest.o: simtest.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
simDemo1.o: simDemo1.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<
		
simDemo2.o: simDemo2.cpp core.h
		$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm -f *.o
	
cleanall: clean
	rm -f simtest simDemo1
	
	
