#include "Moviment.h"

void Moviment::afegeixPosicio(const Posicio& p)
{
	if (nPosicions < MAX_POSICIONS) //nova posicio al cami
    {
        cami[nPosicions++] = p;
    }
}

Posicio Moviment::getUltimaPosicio() const // retorna la ultima posicio del moviment
{
    Posicio resultat;

    if (nPosicions > 0) 
    {
		resultat = cami[nPosicions - 1]; //n-1 es la darrera posicio
    }
    return resultat;
}

void Moviment::mostra() const
{
    for (int i = 0; i < nPosicions; i++) 
    {
		cout << cami[i].getPosicio(); // mostra la posicio del cami
        if (i != nPosicions - 1) 
        {
			cout << " - "; // separador entre posicions
        };
    }
    cout << endl;
}