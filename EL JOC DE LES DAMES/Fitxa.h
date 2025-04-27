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

   void convertirEnDama();  
   void afegeixMovimentValid(const Moviment& m);  
   void netejaMoviments();  
   void mostraMoviments() const;  
   bool esBuida() const { return tipus == TIPUS_EMPTY; }  

   TipusFitxa getTipus() const { return tipus; }  
   void setTipus(TipusFitxa t) { tipus = t; } 
   ColorFitxa getColor() const { return color; }  
   void setColor(ColorFitxa c) { color = c; } 
   int getNumMoviments() const { return nMoviments; }  
   Moviment getMoviment(int i) const { return movimentsValids[i]; }  

private:  
   TipusFitxa tipus;  
   ColorFitxa color;  
   Moviment movimentsValids[MAX_MOVIMENTS];  
   int nMoviments;  
};

#endif
