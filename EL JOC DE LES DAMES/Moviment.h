#ifndef MOVIMENT_H
#define MOVIMENT_H

#include <iostream>
#include "Posicio.h"

const int MAX_POSICIONS = 12;
 
class Moviment {

public:
    Moviment() : nPosicions(0), pecesMates(0) {}

    void setPecesMates(int n) { pecesMates = n; }
    int getPecesMates() const { return pecesMates; }
    int getNumPosicions() const { return nPosicions; }
    Posicio getPosicio(int i) const { return cami[i]; }
    bool esMovimentValid() const { return nPosicions > 0; }

    void afegeixPosicio(const Posicio& p);
    Posicio getUltimaPosicio() const;
    void mostra() const {}

    private:
        Posicio cami[MAX_POSICIONS];
        int nPosicions;
        int pecesMates;
};

#endif
