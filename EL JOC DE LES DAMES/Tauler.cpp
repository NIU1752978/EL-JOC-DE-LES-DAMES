#include "Tauler.h"
#include <fstream>

void Tauler::inicialitza(const string & nomFitxer)
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
    bool hiHaCaptures = false;

    for (int i = 0; i < N_FILES; i++)
    {
        for (int j = 0; j < N_COLUMNES; j++)
        {
            Fitxa& fitxa = m_tauler[i][j];

            if (!fitxa.esBuida())
            {
                Posicio origen(i, j);
                int nPosicions = 0;
                Posicio posicionsPossibles[MAX_POSICIONS];
                getPosicionsPossibles(origen, nPosicions, posicionsPossibles);

                for (int k = 0; k < nPosicions; k++)
                {
                    Moviment moviment;
                    moviment.afegeixPosicio(origen);
                    moviment.afegeixPosicio(posicionsPossibles[k]);

                    if (esCaptura(origen, posicionsPossibles[k]))
                    {
                        moviment.setPecesMates(1);
                        hiHaCaptures = true;
                    }

                    fitxa.afegeixMovimentValid(moviment);
                }
            }
        }
    }

    if (hiHaCaptures)
    {
        for (int i = 0; i < N_FILES; i++)
        {
            for (int j = 0; j < N_COLUMNES; j++)
            {
                Fitxa& fitxa = m_tauler[i][j];
                int nMoviments = fitxa.getNumMoviments();
                Moviment nousMoviments[MAX_MOVIMENTS];
                int comptador = 0;

                for (int k = 0; k < nMoviments; k++)
                {
                    if (fitxa.getMoviment(k).getPecesMates() > 0)
                    {
                        nousMoviments[comptador++] = fitxa.getMoviment(k);
                    }
                }

                fitxa.netejaMoviments();

                for (int k = 0; k < comptador; k++)
                {
                    fitxa.afegeixMovimentValid(nousMoviments[k]);
                }
            }
        }
    }
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti)
{
    bool movimentValid = false;

	Fitxa fitxaOrigen = m_tauler[origen.getFila()][origen.getColumna()]; //obtenim la fitxa d'origen
    
	for (int i = 0; i < fitxaOrigen.getNumMoviments(); i++) //te movimnts vàlids?
    {
        Moviment mov = fitxaOrigen.getMoviment(i);

        if (mov.getUltimaPosicio() == desti) // mirem que la ultima pocisicio de la peça sigui el desti (la ultima)
        {
            movimentValid = true;

			m_tauler[origen.getFila()][origen.getColumna()] = Fitxa(); //netegem la fitxa d'origen

			m_tauler[desti.getFila()][desti.getColumna()] = fitxaOrigen; //assignem la fitxa d'origen al desti

            if (mov.getPecesMates() > 0) //si hi ha captures doncs eliminem la fitxa capturada
            {
				for (int j = 1; j < mov.getNumPosicions(); j++)
				{
					Posicio p1 = mov.getPosicio(j - 1);
					Posicio p2 = mov.getPosicio(j);

					int filaDelMig = (p1.getFila() + p2.getFila()) / 2;
					int columnaDelMig = (p1.getColumna() + p2.getColumna()) / 2;

					m_tauler[filaDelMig][columnaDelMig] = Fitxa(); // ara netegem la fitxa del mig
				}
            }

            if ((fitxaOrigen.getColor() == COLOR_BLANC && desti.getFila() == 0) ||
				(fitxaOrigen.getColor() == COLOR_NEGRE && desti.getFila() == N_FILES - 1)) // el que fem es si arriba a la fila 0 o 7 llavors la fitxa es converteix en dama
            {
                m_tauler[desti.getFila()][desti.getColumna()].setTipus(TIPUS_DAMA);
            }
        }
    }

    return movimentValid;
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[])
{
    nPosicions = 0;
    Moviment movimentsPendents[MAX_MOVIMENTS];
    int numMovimentsPendents = 0;

    Moviment movimentInicial;
    movimentInicial.afegeixPosicio(origen);
    movimentsPendents[numMovimentsPendents++] = movimentInicial;

    while (numMovimentsPendents > 0)
    {
        Moviment movimentActual = movimentsPendents[0];

        for (int i = 0; i < numMovimentsPendents - 1; i++)
        {
            movimentsPendents[i] = movimentsPendents[i + 1];
        }

        numMovimentsPendents--;

        Posicio posicioActual = movimentActual.getUltimaPosicio();

        Posicio posicionsValides[MAX_POSICIONS];
        int nPosicionsValides = 0;
        obtenirPosicionsValides(posicioActual, posicionsValides, nPosicionsValides);

        if (nPosicionsValides == 0 && movimentActual.getNumPosicions() > 1)
        {
            posicionsPossibles[nPosicions++] = posicioActual;
        }
        else
        {
            for (int j = 0; j < nPosicionsValides; j++)
            {
                Posicio novaPosicio = posicionsValides[j];
                Moviment nouMoviment = movimentActual;

                nouMoviment.afegeixPosicio(novaPosicio);

                if (esCaptura(posicioActual, novaPosicio))
                {
                    movimentsPendents[numMovimentsPendents++] = nouMoviment;
                }
                else
                {
                    posicionsPossibles[nPosicions++] = novaPosicio;
                }
            }
        }
    }
}

void Tauler::establirFitxa(const Posicio& posicio, const Fitxa& fitxa)
{
	int fila = posicio.getFila();
    int columna = posicio.getColumna();

    m_tauler[fila][columna] = fitxa;
}

string Tauler::toString() const
{
    string resultat = "";

    for (int fila = 0; fila < N_FILES; fila++)
    {
        char numeroFila = '0' + (N_FILES - fila);
        resultat = resultat + numeroFila + ": ";

        for (int columna = 0; columna < N_COLUMNES; columna++)
        {
            char c = '_';
            Fitxa fitxa = m_tauler[fila][columna];

            if (!fitxa.esBuida())
            {
                if (fitxa.getTipus() == TIPUS_DAMA)
                {
                    if (fitxa.getColor() == COLOR_BLANC)
                    {
                        c = 'D';
                    }
                    else
                    {
                        c = 'R';
                    }
                }
                else
                {
                    if (fitxa.getColor() == COLOR_BLANC)
                    {
                        c = 'O';
                    }
                    else
                    {
                        c = 'X';
                    }
                }
            }

            resultat = resultat + c + " ";
        }

        resultat = resultat + "\n";
    }

    resultat = resultat + "   a b c d e f g h\n";

    return resultat;
}

void Tauler::inicialitzaTaulerVuit()
{
    for (int fila = 0; fila < N_FILES; fila++)
    {
        for (int columna = 0; columna < N_COLUMNES; columna++)
        {
			Fitxa buida;
			buida.setTipus(TIPUS_EMPTY);
            m_tauler[fila][columna] = buida;
        }
    }
}

void Tauler::obtenirPosicionsValides(const Posicio& origen, Posicio posicionsValides[], int& nPosicionsValides)
{
    nPosicionsValides = 0;

    int filaOrigen = origen.getFila();
    int columnaOrigen = origen.getColumna();
    Fitxa& fitxaOrigen = m_tauler[filaOrigen][columnaOrigen];

    int direccionsDiagonals[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    if (fitxaOrigen.getTipus() == TIPUS_DAMA)
    {
        // Dames poden moure en totes les diagonals
        for (int d = 0; d < 4; d++)
        {
            int incrementFila = direccionsDiagonals[d][0];
            int incrementColumna = direccionsDiagonals[d][1];
            int filaActual = filaOrigen + incrementFila;
            int columnaActual = columnaOrigen + incrementColumna;
            bool hiHaEnemic = false;
            bool movimentFinalitzat = false;

            while (!movimentFinalitzat && filaActual >= 0 && filaActual < N_FILES && columnaActual >= 0 && columnaActual < N_COLUMNES)
            {
                if (m_tauler[filaActual][columnaActual].esBuida())
                {
                    posicionsValides[nPosicionsValides++] = Posicio(filaActual, columnaActual);
                    if (hiHaEnemic)
                    {
                        movimentFinalitzat = true;
                    }
                }
                else
                {
                    if (!hiHaEnemic && m_tauler[filaActual][columnaActual].getColor() != fitxaOrigen.getColor())
                    {
                        hiHaEnemic = true;
                    }
                    else
                    {
                        movimentFinalitzat = true;
                    }
                }

                filaActual += incrementFila;
                columnaActual += incrementColumna;
            }
        }
    }
    else if (fitxaOrigen.getTipus() == TIPUS_NORMAL)
    {
        int movimentVertical = 0;
        if (fitxaOrigen.getColor() == COLOR_BLANC)
        {
            movimentVertical = -1;  // els blancs es mouen cap amunt
        }
        else
        {
            movimentVertical = 1;   // els negres cap avall
        }

        for (int movimentHoritzontal = -1; movimentHoritzontal <= 1; movimentHoritzontal += 2)
        {
            int filaDesti = filaOrigen + movimentVertical;
            int columnaDesti = columnaOrigen + movimentHoritzontal;

            if (filaDesti >= 0 && filaDesti < N_FILES && columnaDesti >= 0 && columnaDesti < N_COLUMNES)
            {
                if (m_tauler[filaDesti][columnaDesti].esBuida())
                {
                    posicionsValides[nPosicionsValides++] = Posicio(filaDesti, columnaDesti);
                }
                else if (m_tauler[filaDesti][columnaDesti].getColor() != fitxaOrigen.getColor())
                {
                    // Comprovem si es pot saltar una peça contrària
                    int filaSalt = filaDesti + movimentVertical;
                    int columnaSalt = columnaDesti + movimentHoritzontal;

                    if (filaSalt >= 0 && filaSalt < N_FILES && columnaSalt >= 0 && columnaSalt < N_COLUMNES)
                    {
                        if (m_tauler[filaSalt][columnaSalt].esBuida())
                        {
                            posicionsValides[nPosicionsValides++] = Posicio(filaSalt, columnaSalt);
                        }
                    }
                }
            }
        }
    }
}

bool Tauler::esCaptura(const Posicio& origen, const Posicio& desti)
{
    bool capturem = false;

    int filaOrigen = origen.getFila();
    int columnaOrigen = origen.getColumna();
    int filaDesti = desti.getFila();
    int columnaDesti = desti.getColumna();

    if (abs(filaOrigen - filaDesti) == 2 && abs(columnaOrigen - columnaDesti) == 2)
    {
        int filaDelMig = (filaOrigen + filaDesti) / 2;
        int columnaDelMig = (columnaOrigen + columnaDesti) / 2;

        const Fitxa& fitxaOrigen = m_tauler[filaOrigen][columnaOrigen];
        const Fitxa& fitxaDelMig = m_tauler[filaDelMig][columnaDelMig];

        if (!fitxaDelMig.esBuida() && fitxaDelMig.getColor() != fitxaOrigen.getColor())
        {
            capturem = true;
        }
    }

    return capturem;
}
