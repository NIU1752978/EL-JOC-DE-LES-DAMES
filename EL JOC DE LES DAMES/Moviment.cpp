#include "Moviment.h"

void Moviment::afegeixPosicio(const Posicio& p)
{
    if (nPosicions < MAX_POSICIONS) 
    {
        cami[nPosicions++] = p;
    }
}

Posicio Moviment::getUltimaPosicio() const
{
    Posicio resultat;

    if (nPosicions > 0) 
    {
        resultat = cami[nPosicions - 1];
    }
    return resultat;
}

void Moviment::mostra() const 
{
    for (int i = 0; i < nPosicions; i++) 
    {
        cout << cami[i].getPosicio();
        if (i != nPosicions - 1)
        {
            cout << " -> ";
        };
    }
    cout << endl;
}