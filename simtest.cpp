
#include "simulator.h"



Facility Linka("linka");


class Transakce : public Process
{
    void Behavior()
    {
        Seize(Linka, SLOT(Transakce::Behavior2),1);
    }
    void Behavior2()
    {
        scheduleAt(Time() + Normal(1,0.15), SLOT(Transakce::Behavior3) ); // Wait
    }
    void Behavior3()
    {
        Release(Linka);
    }
};

class Generator : public Event {
    void Behavior() {
        (new Transakce)->scheduleAt(Time());
        scheduleAt(Time() + Exponential(2));
    }
};




int main(int argc, char* argv[])
{
    InitTime(0, 1000);
    (new Generator)->scheduleAt(0);
    Run();

    Linka.Output();


    return 0;
}
