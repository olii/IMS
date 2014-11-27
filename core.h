/* simulator core header */

#ifndef _SIM_H
#define _SIM_H

#include <list>
#include <limits.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <queue>

using std::cout;


class Calendar;
class Facility;
class Store;


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
    std::string name;
    static unsigned int id;
    
public:	
    Statistics();
    Statistics(std::string name_); 
    double Min() const;
    double Max() const;
    double Avg() const;
    double NumRecords() { return numRecords; }
    void Clear() { numRecords = min = max = sum = 0; }
    virtual void Output(); // will be reimplemented in facility, store and queue 
    void Record(double val); // stat recording
};

unsigned int Statistics::id = 0;
    


class TimeStats
{
    double t_0; // start of statistics
    double t_end; // end of statistics
    double sum; // total time of service
    double start_time; // start of service
    std::string name;
    static unsigned int id;
    bool busy;
    double min, max;
    // TODO rozptyl
    
    
public:
    TimeStats();
    TimeStats(std::string name_);
    bool Busy() { return busy; }
    double Avg() const;
    double Min() const {return min;}
    double Max() const {return max;}
    double Start() { return t_0; }
    double End() { return t_end; }
    
    void Init(double t0, double end) { t_0 = t0; t_end = end; }
    void operator()(); // stat recording
  
};

unsigned int TimeStats::id = 0;

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
    }
    virtual ~MetaEntity(){}

    #define SLOT(x) (static_cast<MetaEntity::Fptr>((&x)))
    virtual void Behavior() = 0;
    void scheduleAt( double t, Fptr callback );
    void scheduleAt( double t = 0 ) { MetaEntity::scheduleAt(t, SLOT(MetaEntity::Behavior)); }
    std::string name() { return _name; }
    uint8_t GetProcPrio() { return _prioriy; }
    double activationTime() {return _activationTime;}
    Fptr Passivate();

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

    bool operator<=( QueueItem& item2)
    {
        if ( this->priority < item2.priority ) return false; // servisna priorita
        if ( this->priority == item2.priority )
        {
            if ( this->target_object->GetProcPrio() <= item2.GetTarget().GetProcPrio() )
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
public:
    Queue(){ queue.clear(); }
    bool Empty() {return queue.empty(); }
    void Clear() { queue.clear(); }
    int Length() { return queue.size(); }

    void Insert(QueueItem item);
    QueueItem GetFirst();
    QueueItem& Front();
    bool isPresent( MetaEntity &obj );
    void Dump();
    std::list<QueueItem>& QueueRawAccess();
};


class Facility
{
    Statistics stats;
    TimeStats tStats;
public :
    Facility (std::string name) {
        _name = name; id = Internal::GenerateID();
    }
    Facility () {
        id = Internal::GenerateID(); _name = "Facility_" + std::to_string(id);
    }
    bool Busy() { return (in.GetTargetPtr() == nullptr )?false:true; }
    int QueueLen() { return Q1.Length(); }
    void Seize(MetaEntity *obj, MetaEntity::Fptr callback, uint8_t service_prio = 0 );
    void Release( MetaEntity *obj );

    QueueItem in;
    void Output();

private:
    int id;
    std::string _name;
    Queue Q1;
    Queue Q2;
};


class Store
{
public:
    Store( int _cap = 1 ): capacity(_cap), freeCounter(_cap){
        id = Internal::GenerateID(); _name = "Store_" + std::to_string(id);
    }
    Store( std::string name, int _cap = 1): capacity(_cap), _name(name), freeCounter(_cap){
        id = Internal::GenerateID();
    }

    int Free(){ return freeCounter; }
    int Used(){ return capacity - freeCounter; }
    int Capacity(){ return capacity; }
    bool Full() {  return ( freeCounter == 0 ); }
    bool Empty(){ return capacity == freeCounter; }
    int QueueLen() { return Q.Length(); }
    void Enter(MetaEntity *obj, MetaEntity::Fptr callback, int _capacity );
    void Leave( int capacity );

private:
    int capacity;
    int id;
    std::string _name;
    int freeCounter;
    Queue Q;
};




void Run();
double Time();
void InitTime(double start, double end);

double Exponential(double middle);
double Uniform(double low, double high);
double Random();





#endif
