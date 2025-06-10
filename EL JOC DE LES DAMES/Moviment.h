#ifndef MOVIMENT_H
#define MOVIMENT_H

#include "Posicio.h"
#include <iostream>

class Tauler;
class Fitxa;

const int MAX_POSICIONS = 12;

class Moviment {
public:
    Moviment();

    void afegeixPosicio(const Posicio& pos);
    Posicio getPosicio(int i) const;
    int getNPosicions() const;
    Posicio getDesti() const;
    int getNumDamesCapturades(const Tauler& tauler) const;

private:
    Posicio cami[MAX_POSICIONS];
    int nPosicions;
};

std::ostream& operator<<(std::ostream& os, const Moviment& m);

#endif
