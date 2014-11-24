#include "core.h"
#include <iostream>
#include <random>


using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> dis(1, 2);

class P : public Process
{
public:
    void Behavior()
    {
        cout << "This is behavior" << endl;
        scheduleAt( Time() + 0.001, SLOT(P::Behavior2));
    }
    void Behavior2()
    {
        cout << "This is behavior2" << endl;
        //scheduleAt( Time() + 2, SLOT(P::Behavior));
    }
};




class E : public Event
{
    void Behavior()
    {
        _name = "E";
        cout << "Generator E" << endl;
        (new P)->scheduleAt(Time() + 0.001);
        scheduleAt( Time() + 0.5);
    }
};


int main()
{
    std::vector<Event*> ptrlist;

    int i = 0;
    /*while(  i < 3 )
    {
        double t = dis(gen);
        Event *ptr = new Process;
        ptrlist.push_back(ptr);
        ptr->scheduleAt( t );
        i++;
    }*/
    E *ptr = new E;
    ptr->scheduleAt(0);

    Calendar::instance().Dump();
    InitTime(0, 10);
    Run();

    /*for( auto it: ptrlist )
    {
        delete it;
    }*/

    return 0;
}
