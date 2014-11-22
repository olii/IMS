

#include <iostream>
#include <string>

#include <algorithm>
#include <vector>
#include "calendar.h"





Calendar::Calendar()
{
	data = new calendar_list;
}



#ifdef DEBUG
void Calendar::dumpCalendar()
{
	using namespace std;
	cout << "------------------------\n"; 
	cout  << "Calendar::dumpCalendar()\n";
	cout << "------------------------\n";
	
	vector<double> scheduled_times;

	for(auto it: *data)
	{
		cout << it->name() << "(Scheduled at: " << it->time_start() << ")" << endl;
		scheduled_times.push_back(it->time_start());
	}
	
	cout << "------------------------\n";
	
	if( is_sorted(scheduled_times.begin(),scheduled_times.end()) )
		cout << "Events are sorted: OK" << endl;
	else
		cout << "Events are sorted: FAIL" << endl;
	
	
}
#endif


/* singleton instance */
Calendar* Calendar::instance_ = nullptr;


inline Calendar* Calendar::instance()
{
	if( instance_ == nullptr ) 
      instance_ = new Calendar; 

	return instance_;
}




void Calendar::Schedule( Action* a )
{
	// prazdny kalendar
	if( data->empty() )
	{
		data->push_front(a);
		calendar_size = 1;
	}
	
	// kalendar s jednym prvkom
	else if( calendar_size == 1 )
	{
		// porovnam a vlozim
		if( a->time_start() > (*data->begin())->time_start() )
			data->insert(++(data->begin()),a);
		else
			data->push_front(a);
		calendar_size++;	
	}
	
	else
	{  // 2+ prvkov, hladam kde mam vlozit
		calendar_list::iterator it = data->begin();

		while( it != data->end() )
		{
			if( (*it)->time_start() > a->time_start() )
			{
				data->insert(it,a);
				calendar_size++;
				break;
			}
			it++;
		}
		
		// dosiel som nakoniec a nevlozil som, vlozim na koniec
		if( it == data->end() )
		{
			data->insert(it,a);
			calendar_size++;
		}
	}
}

Calendar::~Calendar()
{ 
	for(auto it: *data )
	{
		delete it;
	}

	delete data;
	std::cerr << "Warning: Calendar destoyed.\n";
}



void Calendar::destroy_instance()
{
	delete instance_; 
	instance_ = nullptr; 
}

