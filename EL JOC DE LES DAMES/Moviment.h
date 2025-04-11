#ifndef MOVIMENT_H
#define MOVIMENT_H

#include <iostream>
#include "Posicio.h"

const int MAX_POSICIONS = 12;

class Moviment {
private:
    Posicio cami[MAX_POSICIONS];
    int nPosicions;
    int pecesMates;

public:
    Moviment() : nPosicions(0), pecesMates(0) {}

    void afegeixPosicio(const Posicio& p) {
        if (nPosicions < MAX_POSICIONS) {
            cami[nPosicions++] = p;
        }
    }

    void setPecesMates(int n) {
        pecesMates = n;
    }

    int getPecesMates() const {
        return pecesMates;
    }

    int getNumPosicions() const {
        return nPosicions;
    }

    Posicio getPosicio(int i) const {
        return cami[i];
    }

    Posicio getUltimaPosicio() const {
        if (nPosicions == 0) return Posicio();
        return cami[nPosicions - 1];
    }

    bool esMovimentValid() const {
        return nPosicions > 0;
    }

    void mostra() const {
        for (int i = 0; i < nPosicions; ++i) {
            std::cout << cami[i].getPosicio();
            if (i != nPosicions - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
};

#endif
