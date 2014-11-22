


#include "transactions.h"
#include "calendar.h"


/*****************************************
 * Action
  ****************************************/


inline void Action::scheduleAt( double t )
{ 
	schedule_time = t; 
	Calendar::instance()->Schedule(this); 
}

Action::~Action()
{
	;
}





/*****************************************
 * Event
  ****************************************/

inline void Event::Activate(double time)
{
	Action::scheduleAt(time);
}



/*****************************************
 * Process
  ****************************************/
 