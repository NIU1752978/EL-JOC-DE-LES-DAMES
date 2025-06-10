#include "Moviment.h"
#include "Tauler.h"

Moviment::Moviment() {
    nPosicions = 0;
}

void Moviment::afegeixPosicio(const Posicio& p) {
    if (nPosicions < MAX_POSICIONS) {
        cami[nPosicions++] = p;
    }
}

int Moviment::getNPosicions() const {
    return nPosicions;
}

Posicio Moviment::getPosicio(int i) const {
    return cami[i];
}

Posicio Moviment::getDesti() const {
    if (nPosicions > 0) {
        return cami[nPosicions - 1];
    }
    return Posicio();
}

int Moviment::getNumDamesCapturades(const Tauler& tauler) const {
    int n = 0;

    for (int i = 0; i < nPosicions - 1; ++i) {
        Posicio p1 = cami[i];
        Posicio p2 = cami[i + 1];

        int filaCap = (p1.getFila() + p2.getFila()) / 2;
        int colCap = (p1.getColumna() + p2.getColumna()) / 2;

        if (abs(p1.getFila() - p2.getFila()) > 1 && abs(p1.getColumna() - p2.getColumna()) > 1) {
            const Fitxa& f = tauler.getFitxa(filaCap, colCap);
            if (f.getTipus() == TIPUS_DAMA) {
                ++n;
            }
        }
    }

    return n;
}

ostream& operator<<(ostream& os, const Moviment& m) {
    for (int i = 0; i < m.getNPosicions(); ++i) {
        os << m.getPosicio(i);
        if (i < m.getNPosicions() - 1)
            os << " - ";
    }
    return os;
}
