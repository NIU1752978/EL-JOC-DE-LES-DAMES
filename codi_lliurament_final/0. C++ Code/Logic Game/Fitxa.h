#ifndef FITXA_H
#define FITXA_H

#include <iostream>
#include "Moviment.h"

const int MAX_MOVIMENTS = 8;

typedef enum {
    TIPUS_NORMAL,
    TIPUS_DAMA,
    TIPUS_EMPTY
} TipusFitxa;

typedef enum {
    COLOR_NEGRE,
    COLOR_BLANC,
	CAP_GUANYADOR
} ColorFitxa;

class Fitxa {

public:
    Fitxa() : m_tipus(TIPUS_EMPTY), m_color(COLOR_BLANC), m_nMoviments(0) {}
    Fitxa(TipusFitxa t, ColorFitxa c) : m_tipus(t), m_color(c), m_nMoviments(0) {}

    void convertirEnDama();
    void afegeixMovimentValid(const Moviment& m);
    void netejaMoviments();
    void mostraMoviments() const;
    bool esBuida() const { return m_tipus == TIPUS_EMPTY; }
    void visualitza(int x, int y) const;

    TipusFitxa getTipus() const { return m_tipus; }
    void setTipus(TipusFitxa t) { m_tipus = t; }
    ColorFitxa getColor() const { return m_color; }
    void setColor(ColorFitxa c) { m_color = c; }
    int getNumMoviments() const { return m_nMoviments; }
    Moviment getMoviment(int i) const { return m_movimentsValids[i]; }

private:
    TipusFitxa m_tipus;
    ColorFitxa m_color;
    Moviment m_movimentsValids[MAX_MOVIMENTS];
    int m_nMoviments;
};

#endif
