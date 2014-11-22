

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H
 
#include <string>
 
 
class Action // metaobject
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



class Event : public Action
{
  public:
	void Activate(double time = 0);
	explicit Event(std::string name) { name_ = name; }
	
};



class Process : public Action
{
 public:	
	void Activate(double time = 0);
	void Passivate();
	Process(std::string name) { name_ = name; }
	
};

 
#endif /* TRANSACTIONS_H */

