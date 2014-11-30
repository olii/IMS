#include "core.h"
#include <iostream>
#include <random>


Facility Linka("linka");
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

class Transakce : public Process
{
    double t = 0;
    void Behavior()
    {
        cout << name () << " entering in time " << Time() << endl;
        Enter(sklad, SLOT(Transakce::Behavior2),1);
    }
    void Behavior2()
    {
        cout << name () << " entered in time " << Time() << endl;
        scheduleAt(Time() + 1, SLOT(Transakce::Behavior3) );
    }
    void Behavior3()
    {
        cout << name () << " leaving in time " << Time() << endl;
        Leave(sklad, 1);
    }
};

class Generator : public Event {
    void Behavior() {
        if(i++ == 3) return;
        if (i == 0)
        {
            (new Transakce)->scheduleAt(Time());
        }
        (new Transakce)->scheduleAt(Time());
        scheduleAt(Time() + 1);
    }
    int i = 0;
};





int main(int argc, char* argv[])
{
    InitTime(0, 10);
    (new Generator)->scheduleAt(0);
    Run();

    sklad.Output();


    return 0;
}
