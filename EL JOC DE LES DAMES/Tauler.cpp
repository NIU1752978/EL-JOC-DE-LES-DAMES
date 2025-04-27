#include "Tauler.h"
#include <fstream>

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
            establirFitxa(pos, fitxa);
        }
    }
    fitxer.close();
}

void Tauler::actualitzaMovimentsValids()
{
    Moviment movimentsValids[100];
    Moviment movimentsPendents[100];
	Moviment movimentActual;
	int nMovimentsPendents = 0;

	movimentsPendents[nMovimentsPendents++] = movimentActual;
	Posicio posicioActual = movimentActual.getUltimaPosicio();

    do
    {
        movimentActual = movimentsPendents[0];
		nMovimentsPendents--;
		posValides = PosicionsValides(posicioActual);

        while (posValides != 0)
        {
			Posicio posicioValid = posValides[0];
			movimentsValids[nMovimentsPendents++] = movimentActual;
			movimentsPendents[nMovimentsPendents++] = movimentActual;
			posicioActual = posicioValid;

            movimentPendents = duplicaMoviment(movimentActual);

        }

	} while (movimentsPendents != 0);
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti)
{
	bool movimentValid = false;
    return movimentValid; 
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[])
{

}

void Tauler::establirFitxa(const Posicio& posicio, const Fitxa& fitxa)
{
}

string Tauler::toString() const
{
    return "";
}

void Tauler::inicialitzaTaulerVuit()
{
}

void Tauler::carregaFitxes(const string& nomFitxer)
{
}
