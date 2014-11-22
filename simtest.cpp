
#include <random>
#include <cstdlib>
#include <iostream>
#include <string>

#include "simulator.h"


class CustomEvent: public Event
{
  public:
	void Behavior()
	{
		std::cout << "I'm here! " << std::endl;
	}

	explicit CustomEvent(std::string name) : Event(name) { }
};





int main(int argc, char* argv[])
{
	using namespace sim;

	if(argc != 2)
	{
		std::cerr << "Usage: ./calendar NUM_EVENTS\n";
		return EXIT_FAILURE;
	}

	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(1, 5);

	
	int i = 0;
	while( i < atoi(argv[1]) )
	{
		std::string name("Event " + std::to_string(i) );
		(new CustomEvent(name))->Activate(dis(gen));
		i++;
	}

	Run();


	
	return EXIT_SUCCESS;
}