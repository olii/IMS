

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
	Action* GetFirst(); 
	void Schedule( Action* a ); // schedule action a at time t
	static Calendar* instance()
	{
		if( instance_ == nullptr ) 
			instance_ = new Calendar; 
		return instance_;
	}

	~Calendar();
	void destroy_instance();
	
	bool empty() { return data->empty(); }
	void delete_first();
	long int Size() { return calendar_size; }
	
#ifdef DEBUG
	void dumpCalendar(); // print current content of calendar to stdout
#endif

  private:
	Calendar();
    long int calendar_size;
	calendar_list* data;  
	static Calendar* instance_;

};



 
class Action // metaobject
{
  protected:
  	double schedule_time;
	std::string name_;
	
  public:
    virtual void Behavior() = 0;
	virtual ~Action();
	void scheduleAt( double t )
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

