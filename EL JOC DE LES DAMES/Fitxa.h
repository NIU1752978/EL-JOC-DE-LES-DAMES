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
	void setTipus(TipusFitxa t) { tipus = t; }
	void setColor(ColorFitxa c) { color = c; }
    int getNumMoviments() const { return nMoviments; }
    Moviment getMoviment(int i) const { return movimentsValids[i]; }
    bool esBuida() const { return tipus == TIPUS_EMPTY; }
    TipusFitxa getTipus() const {
        return tipus;
    }
	void setTipus(TipusFitxa t) {
		tipus = t;
	}

    ColorFitxa getColor() const {
        return color;
    }

    void convertirEnDama() {
        if (tipus == TIPUS_NORMAL) {
            tipus = TIPUS_DAMA;
        }
    }

    void afegeixMovimentValid(const Moviment& m) {
        if (nMoviments < MAX_MOVIMENTS) {
            movimentsValids[nMoviments++] = m;
        }
    }

    void netejaMoviments() {
        nMoviments = 0;
    }

    int getNumMoviments() const {
        return nMoviments;
    }

    Moviment getMoviment(int i) const {
        return movimentsValids[i];
    }

    void mostraMoviments() const {
        for (int i = 0; i < nMoviments; ++i) {
            movimentsValids[i].mostra();
        }
    }

private:
	TipusFitxa tipus;
	ColorFitxa color;
	Moviment movimentsValids[MAX_MOVIMENTS];
	int nMoviments;
};

#endif
