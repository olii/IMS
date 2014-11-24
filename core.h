/* simulator core header */

#ifndef _SIM_H
#define _SIM_H

#include <list>
#include <limits.h>
#include <stdint.h>
#include <string>
#include <iostream>

using std::cout;


class Calendar;

namespace Internal {
    extern long double Time; // simulation time
    extern long double TimeStop;
    int GenerateID();
}

class MetaEntity
{
protected:
    std::string _name;
    int id;
    uint8_t _prioriy = 0; // priorita

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

};

class Process : public MetaEntity
{
public:
    Process(uint8_t prio = 0) : MetaEntity::MetaEntity(prio) {
        _name = "Process " + std::to_string(id);
    }
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
private:
	double activationTime;
	uint8_t priority;
    MetaEntity& target_object;
    MetaEntity::Fptr ptr;
public:
    CalendarItem( MetaEntity &t, MetaEntity::Fptr _ptr, double time, uint8_t _priority = 0 ):
        activationTime(time),
        priority(_priority),
        target_object(t),
        ptr(_ptr)
    {;}

    MetaEntity& GetTarget() { return target_object; }
    MetaEntity::Fptr GetPtr() { return ptr; }
    uint8_t GetPriority() const { return priority; }
    double GetTime()  const { return activationTime; }
    void Execute() { (target_object.*ptr)(); }


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

    void Schedule(MetaEntity &t, MetaEntity::Fptr ptr, double time, uint8_t priority = 0 );


};


void Run();
double Time();
void InitTime(double start, double end);




#endif
