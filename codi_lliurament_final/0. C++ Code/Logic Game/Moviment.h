#ifndef MOVIMENT_H
#define MOVIMENT_H

#include <string>
#include "Posicio.h"

static const int MAX_POSICIONS = 12;
class Tauler;

class Moviment {
public:
    Moviment();

    void afegirPosicio(const Posicio& pos);
    int getNPosicions() const;
    const Posicio& getPosicio(int index) const;
    int getNumDamesCapturades(const Tauler& tauler) const;

    void buida();
    bool estaBuit() const;
    std::string toString() const;
    void fromString(const std::string& linia);
    Posicio getDesti() const;

private:
    Posicio m_posicions[MAX_POSICIONS];
    int m_nPosicions;
};

ostream& operator<<(std::ostream& os, const Moviment& m);
istream& operator>>(std::istream& input, Moviment& m);

#endif
