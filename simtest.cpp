#include "core.h"
#include <iostream>
#include <random>


Facility linka("linka");
Store sklad("sklad");


using namespace std;
/*
class P : public Process
{
public:
    void Behavior()
    {
        cout << name() << endl;
        cout << name() << " Entering store" << endl;
        Enter(sklad, SLOT(P::Behavior1),1);

    }
    void Behavior1()
    {
        cout << name() << " Entered  store" << endl;
        scheduleAt(Time() + 3, SLOT(P::Behavior2));

    }
    void Behavior2()
    {
        cout << name() << " Leaving store" << endl;
        Leave(sklad,1);
    }

    void Behavior()
    {
        cout << name() << endl;
        if ( id == 2 ) _prioriy = 3;
        scheduleAt( Time() + id , SLOT(P::Behavior1));
    }

    void Behavior1()
    {

        cout << name() << ": prepare to seize" << endl;
        Seize(linka, SLOT(P::Behavior2));
    }
    void Behavior2()
    {
        cout << name() << ": seized!" << endl;
        cout << name() << ": WAIT" << endl;
        scheduleAt( Time() + 3, SLOT(P::Behavior3));
    }
    void Behavior3()
    {
        cout << name() << ": Done wait" << endl;
        cout << name() << ": prepare to release" << endl;
        Release(linka);
        cout << name() << ": released" << endl;
        cout << name() << ": EXIT ...." << endl;
    }
};




class E : public Event
{
    void Behavior()
    {
        _name = "E";
        cout << "Generator E" << endl;
        (new P)->scheduleAt(Time());
        scheduleAt( Time() + 0.5);
    }
};*/




int main(int argc, char* argv[])
{
    int i = 0;
    while(  i < 3 )
    {
        double t = Exponential(2);
        P *ptr = new P;
        ptr->scheduleAt( t );
        i++;
    }

    InitTime(0, 20);
    Run();


    return 0;
}
