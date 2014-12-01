#include "core.h"
#include <iostream>

const int p_krav = 100;

Store dojicky("Dojicky", 3);
Facility rampa("Nakladaci rampa");

int konvic = 0; // sdilena promenna - znamena pocet hotovych konvic s mlikem

//Stat nalozeni("Jak dlouho ceka auto na nalozeni");

// cas je v minutach

// proces kravy
class Krava : public Process {
    void Behavior() {
        // zivotni cyklus
        scheduleAt(Time() + Exponential(15*60), SLOT(Krava::Behavior2) ); // 15hod
    }
    void Behavior2() {
        Enter(dojicky, SLOT(Krava::Behavior3), 1); // bere dojicku
    }
    void Behavior3(){
        // doba dojeni
        double time;
        if (Random()<=0.1)
            time = Exponential(15);
        else
            time = Exponential(8);

        scheduleAt(Time() + time, SLOT(Krava::Behavior4));
    }
    void Behavior4(){
        konvic++; // dalsi hotova konvice
        Leave(dojicky, 1); // uvolneni dojicky
        /* infinite loop */
        scheduleAt(Time(), SLOT(Krava::Behavior));
    }
};


// proces auta nakladajiciho a odvazejiciho konvice
class Auto : public Process {
    int nalozenychKonvic = 0;
    void Behavior() {
        nalozenychKonvic = 0;
        Seize(rampa, SLOT(Auto::Behavior2)); // postavi se na rampu
    }
    void Behavior2(){
        // bere 20 konvic
        if (nalozenychKonvic<20 && konvic ){
            konvic--;
            nalozenychKonvic++;
            scheduleAt(Time()+Uniform(1,2), SLOT(Auto::Behavior2)); // nalozi ji
            return;
        }
        if ( !konvic )
            scheduleAt(Time() + 1.0/60.0, SLOT(Auto::Behavior2)); // kazdu sekundu
        else
            scheduleAt(Time(), SLOT(Auto::Behavior3));
    }
    void Behavior3(){
        Release(rampa);
        scheduleAt(Time()+60, SLOT(Auto::Behavior));
    }

};

int main()
{
    InitTime(0,200*60); // 200 hodin casovy ramec

    // vygenerovat 100 krav do systemu (zustavaji tam)
    for (int a=0; a<p_krav; a++)
        (new Krava)->scheduleAt(Time());

    // dve auta do systemu
    (new Auto)->scheduleAt(Time());
    (new Auto)->scheduleAt(Time());

    Run();

    rampa.Output();
    dojicky.Output();
    //nalozeni.Output();
}

