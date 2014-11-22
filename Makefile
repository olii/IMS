all: calendar.o simulator.o transactions.o simtest.o
	g++ -Wall -O2 -std=c++11 calendar.o simulator.o transactions.o simtest.o -o simtest
	
%.o : %.cpp
	g++ -Wall -pedantic -O2 -std=c++11 -c -o $*.o $*.cpp
	
debug:
	g++ -Wall -Wextra -pedantic -g -std=c++11 calendar.cpp -o calendar
	
clean:
	rm calendar