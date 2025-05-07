#include "Fitxa.h"

void Fitxa::convertirEnDama() //peça normal a dama
{
    if (tipus == TIPUS_NORMAL) 
    {
        tipus = TIPUS_DAMA;
    }
}

void Fitxa::afegeixMovimentValid(const Moviment& m) //afegim moviments valids al vector
{
    if (nMoviments < MAX_MOVIMENTS) 
    {
        movimentsValids[nMoviments++] = m; 
    }
}

void Fitxa::netejaMoviments() //netegem els moviments
{
    nMoviments = 0;
}

void Fitxa::mostraMoviments() const //mostrem els moviments
{
    for (int i = 0; i < nMoviments; i++) 
    {
        movimentsValids[i].mostra();
    }
}