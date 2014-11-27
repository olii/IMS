#include "core.h"
#include <iostream>
#include <random>


Facility Linka("linka");
Store sklad("sklad");

int bezCekani = 0;

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
    void Behavior()
    {
        if (!Linka.Busy()) bezCekani++;
        Seize(Linka, SLOT(Transakce::Behavior2));
        //dobaVSystemu(Time - tvstup);
    }
    void Behavior2()
    {
        double obsluha;
        obsluha = Exponential(10);
        scheduleAt(Time() + obsluha, SLOT(Transakce::Behavior3) );
    }
    void Behavior3()
    {
        //dobaObsluhy(obsluha);
        Release(Linka);
        //dobaVSystemu(Time - tvstup);
    }
};

class Generator : public Event {
    void Behavior() {
        (new Transakce)->scheduleAt(Time());
        scheduleAt(Time() + Exponential(11));
    }
};





int main(int argc, char* argv[])
{
    InitTime(0, 100000);
    (new Generator)->scheduleAt(0);
    Run();
/*
    dobaObsluhy.Output();
    dobaVSystemu.Output();
*/
    std::cout << "Bez cekani: " << bezCekani << std::endl;
    Linka.Output();


    return 0;
}
