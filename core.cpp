#include <vector>
#include <algorithm>
#include <random>
#include <cfloat>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
	


#include "core.h"

namespace Internal {
    long double Time = 0; // simulation time
    long double TimeStop = DBL_MAX;

    int GenerateID()
    {
        static int s_nID = 0;
        return s_nID++;
    }
}
unsigned int Statistics::id = 0;
unsigned int TimeStats::id = 0;

void MetaEntity::scheduleAt(double t, Fptr callback)
{
   Calendar::instance().Schedule(*this, callback, t, _prioriy);
   ++referenceCounter;
   _activationTime = t;
}

MetaEntity::Fptr MetaEntity::Passivate()
{
    return Calendar::instance().Passivate(this);
}

MetaEntity::Fptr Calendar::Passivate(MetaEntity *entity)
{
    for( auto it = calendar.begin(); it != calendar.end(); it++ ) {
        if ( it->GetTargetPtr() == entity )
        {
            MetaEntity::Fptr tmp = it->GetPtr();
            it->GetTarget().referenceCounter += 1;
            calendar.erase(it);
            return tmp;
        }
    }
    return nullptr;
}

void Calendar::Dump()
{
    using namespace std;
    cout << "------------------------\n";
    cout  << "Calendar::Dump()\n";
    cout << "------------------------\n";

    vector<double> scheduled_times;

    for(auto &it: calendar)
    {
        cout << it.GetTarget().name() << "(Scheduled at: " << it.GetTime() << " prio=" << int(it.GetPriority()) << ")" << endl;
        scheduled_times.push_back(it.GetTime());
    }

    cout << "------------------------\n";

    if( is_sorted(scheduled_times.begin(),scheduled_times.end()) )
        cout << "Events are sorted: OK" << endl;
    else
        cout << "Events are sorted: FAIL" << endl;
}



void Calendar::Schedule(MetaEntity &t, MetaEntity::Fptr ptr, double time, uint8_t priority)
{
    CalendarItem item(t, ptr, time, priority);

    for( auto it = calendar.begin(); it != calendar.end(); it++ ) {
      if ( *it <= item ) {
          continue;
      }
      else {
          calendar.insert(it, item);
          return;
      }
    }
    calendar.push_back(item);
}


void Run()
{
    std::cout << std::endl << "====== RUN ======" << std::endl;
    cout.precision(6);
    while( !Calendar::instance().Empty() )
    {
        //Calendar::instance().Dump();
        CalendarItem item = Calendar::instance().Next();
        if ( item.GetTime() > Internal::TimeStop )
        {
            Calendar::instance().Clear();
            std::cout << std::endl << "====== TIMEOUT ======" << std::endl;
            break;
        }
        Internal::Time = item.GetTime();
        //std::cout << "-------------------\n";
        //std::cout << item.GetTarget().name() << "(Scheduled at: " << item.GetTime() << " prio=" << int(item.GetPriority()) << ")" << std::endl;
        //std::cout << "Current simulation time: " << std::fixed  << Internal::Time << "\n";
        //std::cout << "process has reference counter: " << item.GetTarget().referenceCounter << std::endl;
        item.GetTarget().referenceCounter--;
        item.Execute(); // event->Behavior();
        //std::cout << "process has reference counter: " << item.GetTarget().referenceCounter << std::endl;
        //std::cout << "-------------------\n";
        if (item.GetTarget().referenceCounter == 0)
        {
            GarbageCollector::instance().removePtr(&item.GetTarget());
            delete &item.GetTarget();
        }
    }
    Internal::Time =Internal::TimeStop;
    GarbageCollector::instance().Free();
    std::cout << std::endl;
}

double Time()
{
    return Internal::Time;
}


void InitTime(double start, double end)
{
    Internal::Time = start;
    Internal::TimeStop = end;
}


void Queue::Insert(QueueItem item)
{
    for( auto it = queue.begin(); it != queue.end(); it++ ) {
      if ( *it <= item ) {
          continue;
      }
      else {
          queue.insert(it, item);
          return;
      }
    }
    queue.push_back(item);
}

QueueItem Queue::GetFirst()
{
    QueueItem tmp = queue.front();
    queue.pop_front();
    return tmp;
}

QueueItem &Queue::Front()
{
    return queue.front();
}

bool Queue::isPresent(MetaEntity &obj)
{
    for( auto &it:queue )
    {
        if (&(it.GetTarget()) == &(obj)) return true;
    }
    return false;
}


void Queue::Dump()
{
    using namespace std;
    cout << "------------------------\n";
    cout  << "Queue::Dump()\n";
    cout << "------------------------\n";


    for(auto &it: queue)
    {
        cout << it.GetTarget().name() << "(procprio: " << int(it.GetTarget().GetProcPrio()) << " servprio=" << int(it.GetPriority()) << ")" << endl;
    }

    cout << "------------------------\n";
}

std::list<QueueItem> &Queue::QueueRawAccess()
{
    return queue;
}


void Facility::Seize(MetaEntity *obj, MetaEntity::Fptr callback, uint8_t service_prio)
{
    /* Simlib rewriten method */
    stats.Record(1);
    if ( !Busy() )
    {
        tStats();
        in = QueueItem(*obj, callback,service_prio );
        obj->referenceCounter++;
        in.GetTarget().scheduleAt(Time(), in.GetPtr());
        return;
    }
    if ( service_prio > in.GetPriority()  )
    {
        in.remainingTime = in.GetTarget().activationTime() - Time();
        MetaEntity::Fptr nextcall = in.GetTarget().Passivate();
        in.setPtr(nextcall);
        Q2.Insert(in);
        tStats(); // -- vypnem tstats pre aktualneho

        in = QueueItem(*obj, callback,service_prio );
        obj->referenceCounter++;
        in.GetTarget().scheduleAt(Time(), in.GetPtr());
        tStats(); // -- zapnem tstats pre noveho
    }
    else
    {
        Q1.Insert(QueueItem(*obj, callback,service_prio ));
        obj->Passivate();
        obj->referenceCounter++;
    }
    //Q1.Dump();
    //Q2.Dump();
    //cout << "Queue status Q1:" << Q1.Length() << ", Q2:" << Q2.Length() << "\n";
}

void Facility::Release(MetaEntity */*obj*/)
{
    /* Simlib rewriten method */
    tStats();
    in.GetTarget().referenceCounter--;
    in.resetPtr();
    bool flag = false;
    if (!(Q1.Empty() || Q2.Empty())) {
        flag = Q1.Front().GetPriority() > Q2.Front().GetPriority();
    }
    if (!flag && !Q2.Empty())  // interrupt queue not empty
    {                           // seize from interrupt queue...
        in = Q2.GetFirst();// seize again
        tStats();
        in.GetTarget().scheduleAt(Time() + in.remainingTime, in.GetPtr());
        //Q1.Dump();
        // Q2.Dump();
        //cout << "Queue status Q1:" << Q1.Length() << ", Q2:" << Q2.Length() << "\n";
        return;
    }
    if (!Q1.Empty()) {         // input queue not empty -- seize from Q1
        in = Q1.GetFirst();// seize again
        tStats();
        in.GetTarget().scheduleAt(Time(), in.GetPtr());
        //Q1.Dump();
        //Q2.Dump();
        //cout << "Queue status Q1:" << Q1.Length() << ", Q2:" << Q2.Length() << "\n";
        return;
    }
    //Q1.Dump();
    //Q2.Dump();
}

void Facility::Output()
{
    using namespace std;
    cout << "+---------------------------+" << endl
         << "FACILITY " << _name << endl
         << "+---------------------------+" << endl
         << "Status: " << (Busy() ? "Busy" : "not BUSY") << endl
         << "Time interval: " << tStats.Start() << " - " << (tStats.End() == -1 ? Time() : tStats.End())  << endl
         << "Number of requests: " << stats.NumRecords() << endl
         << "Average utilization: " << tStats.Avg() << endl;

}

void Process::Seize(Facility &f, Fptr callback, uint8_t servicePrio)
{
    f.Seize(this, callback, servicePrio);
}

void Process::Release(Facility &f)
{
    f.Release(this);
}

void Process::Enter(Store &s, MetaEntity::Fptr callback, int capacity)
{
    s.Enter(this, callback, capacity);
}

void Process::Leave(Store &s, int capacity)
{
    s.Leave(capacity);
}



void Store::Enter(MetaEntity *obj, MetaEntity::Fptr callback, int _capacity)
{
    //if ( _capacity > capacity ) abort();
    if ( Free() < capacity )
    {
        QueueItem item = QueueItem(*obj, callback );
        item.requiredCapacity = _capacity;
        Q.Insert(item);
        obj->referenceCounter += 1;
        obj->Passivate();
        return;
    }
    freeCounter -= capacity;
    obj->scheduleAt(Time(), callback);
}

void Store::Leave(int capacity)
{
    freeCounter += capacity;
    if (Q.Empty()) return;
    /* iterate through Q and get first with required capacity < freeCapacity */
    std::list<QueueItem> &queue = Q.QueueRawAccess();

    for( auto it = queue.begin(); it != queue.end(); it++ ) {
      if ( it->requiredCapacity <= Free() ) {
          freeCounter -= it->requiredCapacity; // enter
          it->GetTarget().scheduleAt(Time(), it->GetPtr()); // activate
          queue.erase(it); // remove from queue
          it->GetTarget().referenceCounter--;
          break;
      }
    }
    // didnt find anyone with required capacity < freeCapacity
}


/** Functions for generating random numbers **/
std::mt19937& _random()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

double Random()
{
	return Uniform(0,1);
}

double Exponential(double middle)
{
	std::exponential_distribution<double> distribution(1.0/middle);
	return distribution(_random()); 
}


double Uniform(double low, double high)
{
	if( low >= high )
		throw std::domain_error("Uniform: Range error");

	std::uniform_real_distribution<double> dis(low, high);
	return dis(_random());
}




/** Statistics **/
Statistics::Statistics( std::string name_): 
numRecords(0), min(0), max(0),  sum(0)
{
    name = name_;
}

Statistics::Statistics(): 
numRecords(0), min(0), max(0),  sum(0)
{
    name = std::string("Stat ") + std::to_string(++id);
}


double Statistics::Min() const
{
    if( numRecords != 0)
        return min;
    else
        throw std::logic_error("Statistics " + name + ": No records.");
}


double Statistics::Max() const
{
    if( numRecords != 0)
        return max;
    else
        throw std::logic_error("Statistics " + name + ": No records.");
}

double Statistics::Avg() const
{
    if( numRecords != 0)
        return sum/numRecords;
    else
        throw std::logic_error("Statistics " + name + ": No records.");
}


void Statistics::Output()
{   
    using namespace std;
    cout << "+-------------+" << endl;
    cout << name + " output (TODO)" << endl;
    cout << "+-------------+" << endl;
}


void Statistics::Record(double val)
{
    sum += val;
    if(++numRecords == 1) 
        min = max = val;
    else 
    {
       if(val<min) 
          min = val;
       if(val>max) 
          max = val;
    }
}

void GarbageCollector::Free(){
    //std::cout << "GarbageCollector Free()\n";
    //std::cout << "database count " << database.size() << "\n";
    for( auto it = database.begin(); it != database.end(); it++ ) {
        //std :: cout << "Deleting object" << (*it)->name() << "\n";
        delete *it;
    }

    database.clear();
}

/** TimeStats **/
TimeStats::TimeStats()
{
    t_0 = sum = min = max = 0;
    start_time = t_end = -1;
    busy = false;
    name = std::string("TimeStat ") + std::to_string(++id);
    
}


 TimeStats::TimeStats(std::string name_) :
  t_0(0), t_end(-1), sum(0), 
  start_time(-1), busy(false),
  min(0), max(0)
 {
    name = name_;
 }

void TimeStats::operator()()
{
   // std::cout << "Time() " << Time() << " start_time " << start_time << "t_end: " << t_end << std::endl; 
    if(t_end!=-1 && Time() > t_end)
      return;

    if(start_time == -1) // start of service
    {
        start_time = Time();
        busy = true;
    }
    else // end of service
    {
        double time_of_service = Time() - start_time;
        if(time_of_service<min) min = time_of_service;
        if(time_of_service>max) max = time_of_service;
        
        
        sum += (Time() - start_time);
        start_time = -1;
        busy = false;
    }
}

double TimeStats::Avg() const
{
    if(start_time == -1) // noone in service
         return sum/Time();
         
    else // somebody in service
        return (sum + Time() - start_time) / Time();
}
