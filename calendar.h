
#ifndef CALENDAR_H
#define CALENDAR_H
 
 
 #include <list>
 #include "transactions.h"
 
#define DEBUG

/*namespace Internal {
	size_t Time = 0; // simulation time
}*/






typedef std::list<Action*> calendar_list;


class Calendar
{
  public:
	Action* GetFirst(); // remove first item from calendar
	void Schedule( Action* a ); // schedule action a at time t
	static Calendar* instance();
	void clean();
	~Calendar();
	
	void destroy_instance();
	
	
#ifdef DEBUG
	void dumpCalendar(); // print current content of calendar to stdout
#endif

  private:
	Calendar();
    size_t calendar_size;
	calendar_list* data;  
	static Calendar* instance_;

};

#endif /* CALENDAR_H */



