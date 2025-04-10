#include "Tauler.h"
#include <fstream>

void Tauler::inicialitza(const string& nomFitxer)
{
	ifstream fitxer(nomFitxer);

	if (fitxer.is_open())
	{
		char tipus;
		string posicio;

		while (fitxer >> posicio >> tipus)
		{
			Fitxa fitxa;

			if (tipus == 'D')
			{
				fitxa = DAMA_BLANCA;
			}
			else if (tipus == 'R')
			{
				fitxa = DAMA_NEGRA;
			}
			else if (tipus == 'X')
			{
				fitxa = FITXA_NORMAL_NEGRA;
			}
			else if (tipus == 'O')
			{
				fitxa = FITXA_NORMAL_BLANCA;
			}

			Posicio pos(posicio);
			setFitxa(pos, fitxa);
		}
	}
	fitxer.close();
}

void Tauler::actualitzaMovimentsValids()
{
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti)
{
	return false;
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[])
{
}

string Tauler::toString() const
{
	return string();
}

void Tauler::inicialitzaTaulerVuit()
{
}

bool Tauler::posicioValida(const Posicio& posicio)
{
	return false;
}

void Tauler::carregaFitxes(const string& nomFitxer)
{
}
