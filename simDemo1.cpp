#include "core.h"
#include <iostream>

const double jedna_cesta = 4.0;

Store Kotvy("Sklad kotev", 40);
Facility Stanoviste("Stavoviste");

//Histogram dobaCesty("Doba stravena lyzarem u vleku", 0, 1, 15);
//Histogram pocetPokusu("Pocet pokusu nastoupit", 1, 1, 10);

//Stat	cekaniZavodniku("Doba cekani zavodniku");


// proces volne ujizdejici kotvy
class KotvaBezi : public Process {
public:
    KotvaBezi(int t) : Process() { T=t; } // T=1 - jedna cesta, T=2 - cesta tam a zpet
    void Behavior() {
        scheduleAt(Time() + jedna_cesta*T, SLOT(KotvaBezi::VratKotvu) );
    }

    void VratKotvu() {
        Leave(Kotvy, 1);
    }
    int T;
};



class Lyzar : public Process {
public:
    Lyzar(int pri) : Process(pri) {}

    void Behavior() {
        Seize(Stanoviste, SLOT(Lyzar::StanovisteSeized));
    }
    void StanovisteSeized() {
        //opak:
        Enter(Kotvy, SLOT(Lyzar::EnterKotvy), 1);

    }
    void EnterKotvy()
    {
        scheduleAt(Time() + (0.5), SLOT(Lyzar::WaitContinue));
    }
    void WaitContinue(){
        if (Random()<=0.1) {
            // nezdareny start, kotva jede sama dve cesty (nahoru, dolu)
            (new KotvaBezi(2))->scheduleAt(Time());
            //goto opak
            StanovisteSeized();
            return;
        }
        Release(Stanoviste);

        scheduleAt(Time() + jedna_cesta, SLOT(Lyzar::KotvaDole) );
    }
    void KotvaDole(){
        (new KotvaBezi(1))->scheduleAt(Time()); // nahore opousti kotvu a ta jede sama dolu do skladu
    }
};


// generuje dva typy lyzaru. Obecny predpis
class	Generator : public Event {
public:
    Generator(double interv, int pri) : Event() {
        Interval = interv;
        Pri = pri;
    }

    void Behavior() {
        (new Lyzar(Pri))->scheduleAt(Time());
        scheduleAt(Time() + 1);
    }

    double	Interval;
    int	Pri;
};



int	main()
{
    InitTime(0, 1000);
    (new Generator(1,0))->scheduleAt(Time());
    (new Generator(10,1))->scheduleAt(Time());
    Run();

    Kotvy.Output();
    Stanoviste.Output();
}
