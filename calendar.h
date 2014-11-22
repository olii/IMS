

#include <list>

#define DEBUG




class Facility
{
	/* ??? */
};


class Action
{
  protected:
  	double schedule_time;
	std::string name_;
	virtual void Behavior() = 0;
	
	
  public:
	virtual ~Action();
	void scheduleAt( double t );
	std::string name() { return name_; }
	double time_start() { return schedule_time; }
	
	
};

typedef std::list<Action*> calendar_list;


class Calendar
{
  public:
	Action* GetFirst(); // remove first item from calendar
	void Schedule( Action* a ); // schedule action a at time t
	static Calendar* instance();
	void clean();
	~Calendar();
	Calendar();
	void destroy_instance();
	
	
#ifdef DEBUG
	void dumpCalendar(); // print current content of calendar to stdout
#endif

  private:
    size_t calendar_size;
	calendar_list* data;  
	static Calendar* instance_;

};


namespace Internal {
	size_t Time = 0; // simulation time
}


