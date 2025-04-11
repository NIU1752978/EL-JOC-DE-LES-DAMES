#include "Tauler.h"
#include <fstream>

void Tauler::setFitxa(const Posicio& posicio, const Fitxa& fitxa)
{
}

void Tauler::inicialitza(const string& nomFitxer)
{
	ifstream fitxer(nomFitxer);

	if (fitxer.is_open())
	{
		char tipus;
		string posicio;

		while (fitxer >> tipus >> posicio)
		{
			Fitxa fitxa;

			switch (tipus)
            {
                case 'O':
                    fitxa.setTipus(TIPUS_NORMAL);
                    fitxa.setColor(COLOR_BLANC);
                    break;
                case 'X':
                    fitxa.setTipus(TIPUS_NORMAL);
                    fitxa.setColor(COLOR_NEGRE);
                    break;
                case 'D':
                    fitxa.setTipus(TIPUS_DAMA);
                    fitxa.setColor(COLOR_BLANC);
                    break;
                case 'R':
                    fitxa.setTipus(TIPUS_DAMA);
                    fitxa.setColor(COLOR_NEGRE);
                    break;
                default:
                    fitxa.setTipus(TIPUS_EMPTY);
                    break;
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
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[])
{
}

string Tauler::toString() const
{
}

void Tauler::inicialitzaTaulerVuit()
{
}

bool Tauler::posicioValida(const Posicio& posicio)
{
}

void Tauler::carregaFitxes(const string& nomFitxer)
{
}
