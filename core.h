

#ifndef CORE_H
#define CORE_H
 
#include <string>
#include <list>
#define DEBUG
 
class Action;
 
 
typedef std::list<Action*> calendar_list;


class Calendar
{
  public:
	Action* GetFirst(); // remove first item from calendar
	void Schedule( Action* a ); // schedule action a at time t
	static Calendar* instance()
	{
		if( instance_ == nullptr ) 
			instance_ = new Calendar; 
		return instance_;
	}
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



 
class Action // metaobject
{
  protected:
  	double schedule_time;
	std::string name_;
	virtual void Behavior() = 0;
	
	
  public:
	virtual ~Action();
	void scheduleAt( double t );
	{ 
		schedule_time = t; 
		Calendar::instance()->Schedule(this); 
	}


	std::string name() { return name_; }
	double time_start() { return schedule_time; }
	
};



class Event : public Action
{
  public:
	void Activate(double time = 0) { Action::scheduleAt(time); }
	explicit Event(std::string name) { name_ = name; }
	
};



class Process : public Action
{
 public:	
	void Activate(double time = 0);
	void Passivate();
	Process(std::string name) { name_ = name; }
	
};





 /* Simulator */
void Run();


 
#endif /* CORE_H */

