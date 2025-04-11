#ifndef FITXA_H
#define FITXA_H

#include <iostream>
#include "Moviment.h"

const int MAX_MOVIMENTS = 20;

typedef enum {
    TIPUS_NORMAL,
    TIPUS_DAMA,
    TIPUS_EMPTY
} TipusFitxa;

typedef enum {
    COLOR_NEGRE,
    COLOR_BLANC
} ColorFitxa;

class Fitxa {

public:

    Fitxa() : tipus(TIPUS_EMPTY), color(COLOR_BLANC), nMoviments(0) {}
    Fitxa(TipusFitxa t, ColorFitxa c) : tipus(t), color(c), nMoviments(0) {}

    TipusFitxa getTipus() const { return tipus; }
    ColorFitxa getColor() const { return color; }
    int getNumMoviments() const { return nMoviments; }
    Moviment getMoviment(int i) const { return movimentsValids[i]; }
    bool esBuida() const { return tipus == TIPUS_EMPTY; }

    void convertirEnDama();
    void afegeixMovimentValid(const Moviment& m);
    void netejaMoviments();
    void mostraMoviments() const;
    
private:
	TipusFitxa tipus;
	ColorFitxa color;
	Moviment movimentsValids[MAX_MOVIMENTS];
	int nMoviments;

};

#endif
