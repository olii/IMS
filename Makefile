
all:
	g++ -Wall -O2 -std=c++11 calendar.cpp -o calendar
	
debug:
	g++ -Wall -Wextra -pedantic -g -std=c++11 calendar.cpp -o calendar
	
clean:
	rm calendar