/* simulator core header */

#ifndef _SIM_H
#define _SIM_H

#include <list>
#include <limits.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <exception>
#include <cfloat>

using std::cout;


void Run();
double Time();
void InitTime(double start, double end);

double Exponential(double middle);
double Uniform(double low, double high);
double Random();
double Normal(double middle, double sigma);


class Calendar;
class Facility;
class Store;
class GarbageCollector;
class MetaEntity;

class GarbageCollector
{
public:
    void  addPtr( MetaEntity *ptr ){ database.insert(ptr); }
    void removePtr( MetaEntity *ptr ){ database.erase(ptr); }
    static GarbageCollector& instance(){
        static GarbageCollector instance; // Guaranteed to be destroyed.
        return instance;
    }
    void Free();
    bool flag = false;

private:
    std::unordered_set<MetaEntity*> database;
    explicit GarbageCollector() {}
};


namespace Internal {
    extern long double Time; // simulation time
    extern long double TimeStop;
    int GenerateID();

}


class Statistics
{
    unsigned numRecords;
    double min;
    double max;
    double sum;
    double sum2x;
    std::string name;
    static unsigned int id;
    
public:	
    Statistics(){Statistics("STATISTICS_" + std::to_string(Statistics::id++));}
    Statistics(std::string name_); 
    double Min() const;
    double Max() const;
    double Avg() const;
    double StdDev();
    double NumRecords() { return numRecords; }
    void SetNumRecords(unsigned int val) { numRecords = val; }
    void Clear() { numRecords = min = max = sum = sum2x = 0; }
    void Output();
    void Record(double val); // stat recording
};

class Histogram
{
public:
    Histogram(std::string _name, double _low, double _step, unsigned _count):
        low(_low), step(_step), count(_count), name(_name)
    {
        data.resize(count+2);
    }
    void Sample(double x);
    double Low() {return low;}
    double High() {return low + step*count;}
    double Step() {return step;}
    int Count() {return count;}
    void Output();

   
private:
    std::vector<int> data;
    double low;
    double step;
    int count;
    std::string name;
    Statistics stat;
};


    


class TimeStats
{
public:
    static unsigned int id;

    unsigned int n; // number of samples
    double t_0; // start of statistics
    double sum; // total
    double start_time; // start of statistics
    double min, max;
    // TODO rozptyl

    /*helper*/
    double previousTime;
    double previousValue;
    std::string name;
    
    
public:
    TimeStats(){
        TimeStats("TimeStat_" + std::to_string(TimeStats::id++));
    }
    TimeStats(std::string name_) ;
    double Avg() const;
    double Min() const {return min;}
    double Max() const {return max;}
    double Start() { return t_0; }
    void SampleBegin(double x);
    void SampleEnd();
  
};

class MetaEntity
{
protected:
    std::string _name;
    int id;
    uint8_t _prioriy = 0; // priorita procesu
    double _activationTime = 0; // next call in calendar == activationtime

public:
    typedef void (MetaEntity::*Fptr)();
    MetaEntity(uint8_t prio = 0) : _prioriy(prio) { // zaregistrovat do GC alebo nastavit priznak
        id = Internal::GenerateID();
        _name = std::string("MetaEntity_") + std::to_string(id);
        GarbageCollector::instance().addPtr(this);
    }

    virtual ~MetaEntity(){ }

    #define SLOT(x) (static_cast<MetaEntity::Fptr>((&x)))
    virtual void Behavior() = 0;
    void scheduleAt( double t, Fptr callback );
    void scheduleAt( double t = 0 ) { MetaEntity::scheduleAt(t, SLOT(MetaEntity::Behavior)); }
    std::string name() { return _name; }
    uint8_t GetProcPrio() { return _prioriy; }
    double activationTime() {return _activationTime;}
    Fptr Passivate();

    int referenceCounter = 0;
};

class Process : public MetaEntity
{
public:
    Process(uint8_t prio = 0) : MetaEntity::MetaEntity(prio) {
        _name = "Process " + std::to_string(id);
    }
    void Seize(Facility &f, Fptr callback, uint8_t servicePrio = 0);
    void Release(Facility &f);
    void Enter(Store &s, Fptr callback, int capacity );
    void Leave(Store &s, int capacity);
private:
};




class Event : public MetaEntity
{
public:
    Event(uint8_t prio = 0) : MetaEntity::MetaEntity(prio) {
        _name = "Event " + std::to_string(id);
    }
};



/*
class Event:MetaEvent
class Process:MetaEvent
*/


class CalendarItem
{
protected:
	double activationTime;
	uint8_t priority;
    MetaEntity* target_object = nullptr;
    MetaEntity::Fptr ptr;
public:
    CalendarItem(){}
    CalendarItem( MetaEntity &t, MetaEntity::Fptr _ptr, double time, uint8_t _priority = 0 ):
        activationTime(time),
        priority(_priority),
        target_object(&t),
        ptr(_ptr)
    {;}
    virtual ~CalendarItem() {}

    MetaEntity& GetTarget() { return *target_object; }
    MetaEntity* GetTargetPtr() { return target_object; }
    MetaEntity::Fptr GetPtr() { return ptr; }
    uint8_t GetPriority() const { return priority; }
    double GetTime()  const { return activationTime; }
    void Execute() { (target_object->*ptr)(); }


    inline bool operator<=(const CalendarItem& rhs){
        if ( this->GetTime() < rhs.GetTime() )
            return true;
        else if (this->GetTime() == rhs.GetTime())
        {
            if (this->GetPriority() <= rhs.GetPriority())
                return true;
            else
                return false;
        }
        else
            return false;
    }
};


class Calendar
{
private:
    std::list<CalendarItem> calendar;
    explicit Calendar() { calendar.clear(); }
public:
    static Calendar& instance()
    {
        static Calendar instance; // Guaranteed to be destroyed.
        return instance;
    }

    bool Empty() {return calendar.empty(); }
    void Clear() { calendar.clear(); }
    int Length() { return calendar.size(); }

    CalendarItem Next() {
        CalendarItem tmp = calendar.front();
        calendar.pop_front();
        return tmp;
    }
    void Dump();
    MetaEntity::Fptr Passivate( MetaEntity* entity);

    void Schedule(MetaEntity &t, MetaEntity::Fptr ptr, double time, uint8_t priority = 0 );


};

class QueueItem : public CalendarItem
{
public:
    QueueItem(){}
    QueueItem( MetaEntity &t, MetaEntity::Fptr _ptr, uint8_t _priority = 0 ):
        CalendarItem( t, _ptr, 0, _priority )
    {;}

    void resetPtr() { target_object = nullptr; }
    void setPtr(MetaEntity::Fptr callback) { ptr = callback; }

    double remainingTime = 0; // Q2 in facility
    int requiredCapacity = 0; // Q in store
    double insertTime = 0;

    bool operator<=( QueueItem& item2)
    {
        if ( this->priority < item2.priority ) return false; // servisna priorita
        if ( this->priority == item2.priority )
        {
            if ( this->target_object->GetProcPrio() < item2.GetTarget().GetProcPrio() )
                return false;
            else
                return true;
        }
        else return true;
    }
};


class Queue
{
private:
    std::list<QueueItem> queue;
    std::string _name;
    unsigned int incoming = 0;
    unsigned int outcoming = 0;
    TimeStats tstats;
    Statistics stat;
public:
    Queue(){
        _name = "Queue_" + std::to_string(Internal::GenerateID());
    }
    Queue(std::string name): _name(name){ }
    bool Empty() {return queue.empty(); }
    void Clear() { queue.clear(); }
    int Length() { return queue.size(); }
    std::string name() {return _name;}
    void Insert(QueueItem item);
    QueueItem GetFirst(int capacity = 0);
    QueueItem& Front();
    bool isPresent( MetaEntity &obj );
    void Dump();
    bool hasOutput (){return incoming;}
    void Output();

};


class Facility
{
public :
    Facility (std::string name):
        _name (name),
        Q1(_name + ".Q1"),
        Q2(_name + ".Q2")
    {}
    Facility ():
        id (Internal::GenerateID()),
        _name ("Facility_" + std::to_string(id)),
        Q1(_name + ".Q1"),
        Q2(_name + ".Q2")
    {}
    bool Busy() { return (in.GetTargetPtr() == nullptr )?false:true; }
    int QueueLen() { return Q1.Length(); }
    void Seize(MetaEntity *obj, MetaEntity::Fptr callback, uint8_t service_prio = 0 );
    void Release( MetaEntity *obj );

    void Output();

    QueueItem in;

private:
    int id;
    std::string _name;
    Queue Q1;
    Queue Q2;
    Statistics stats;
    TimeStats tStats;
};


class Store
{
public:
    Store( int _cap = 1 ):
        capacity(_cap),
        id (Internal::GenerateID()),
        _name("Store_" + std::to_string(id)),
        Q(_name + ".Q")
    {
        freeCounter = capacity;
    }
    Store( std::string name, int _cap = 1):
        capacity(_cap),
        _name(name),
        Q(_name + ".Q")
    {
        freeCounter = capacity;
        id = Internal::GenerateID();
    }

    int Free(){ return freeCounter; }
    int Used(){ return capacity - freeCounter; }
    int Capacity(){ return capacity; }
    bool Full() {  return ( freeCounter == 0 ); }
    bool Empty(){ return capacity == freeCounter; }
    std::string name(){return _name;}
    int QueueLen() { return Q.Length(); }
    void Enter(MetaEntity *obj, MetaEntity::Fptr callback, int _capacity );
    void Leave( int _capacity );
    void Output();

private:
    int capacity;
    int id;
    std::string _name;
    int freeCounter;
    Queue Q;
    TimeStats tstats;

    int enterCount = 0;
};





#endif
