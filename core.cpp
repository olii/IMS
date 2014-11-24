#include "core.h"
#include <vector>
#include <algorithm>
#include <float.h>
#include <iostream>


namespace Internal {
    long double Time = 0; // simulation time
    long double TimeStop = DBL_MAX;

    int GenerateID()
    {
        static int s_nID = 0;
        return s_nID++;
    }
}

void MetaEntity::scheduleAt(double t, Fptr callback)
{
    {
      Calendar::instance().Schedule(*this, callback, t, _prioriy); // TODO Priority
    }
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
        CalendarItem item = Calendar::instance().Next();
        Internal::Time = item.GetTime();
        if ( Internal::Time > Internal::TimeStop )
        {
            Calendar::instance().Clear();
            std::cout << std::endl << "====== TIMEOUT ======" << std::endl;
            return;
        }
        //std::cout << item.GetTarget().name() << "(Scheduled at: " << item.GetTime() << " prio=" << int(item.GetPriority()) << ")" << std::endl;
        std::cout << "Current simulation time: " << std::fixed  << Internal::Time << " ";
        item.Execute(); // event->Behavior();

    }
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
