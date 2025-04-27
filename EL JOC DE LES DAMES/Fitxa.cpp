#include "Fitxa.h"

void Fitxa::convertirEnDama() 
{
    if (tipus == TIPUS_NORMAL) {
        tipus = TIPUS_DAMA;
    }
}

void Fitxa::afegeixMovimentValid(const Moviment& m) {
    if (nMoviments < MAX_MOVIMENTS) 
    {
        movimentsValids[nMoviments++] = m;
    }
}

void Fitxa::netejaMoviments() 
{
    nMoviments = 0;
}

void Fitxa::mostraMoviments() const 
{
    for (int i = 0; i < nMoviments; i++) 
    {
        movimentsValids[i].mostra();
    }
}