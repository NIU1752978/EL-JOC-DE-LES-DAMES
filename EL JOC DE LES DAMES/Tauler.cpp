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
    for (int i = 0; i < N_FILES; i++) //netegem tots els moviments
    {
        for (int j = 0; j < N_COLUMNES; j++)
        {
			m_tauler[i][j].netejaMoviments();
        }
    }

    bool hiHaCaptures = false;
	
    for (int i = 0; i < N_FILES; i++)
    {
        for (int j = 0; j < N_COLUMNES; j++)
        {
			Fitxa& fitxa = m_tauler[i][j];

            if (!fitxa.esBuida()) //si la fitxa no es buida obtenim les posicions posibles
            {
				Posicio origen(i, j);
                int nPosicions = 0;
                Posicio posicionsPossibles[MAX_POSICIONS];
				getPosicionsPossibles(origen, nPosicions, posicionsPossibles);

                for (int k = 0; k < nPosicions; k++) //posicions possibles
                {
					Moviment moviment;
                    moviment.afegeixPosicio(origen);
                    moviment.afegeixPosicio(posicionsPossibles[k]);

                    int diferenciaFila = origen.getFila() - posicionsPossibles[k].getFila();
                    int diferenciaColumna = origen.getColumna() - posicionsPossibles[k].getColumna();

                    if (diferenciaFila == 2 || diferenciaFila == -2 && diferenciaColumna == 2 || diferenciaColumna == -2) // si diferencia = 2 absolut
                    {
                        moviment.setPecesMates(1);
						hiHaCaptures = true;
                    }

                    fitxa.afegeixMovimentValid(moviment);
                }
            }
        }
    }

    if (hiHaCaptures == true) //quan hi hagi captures eliminarem els movimnets que no impliquin captura
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
                        nousMoviments[comptador] = fitxa.getMoviment(k);
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

    if (!origen.posicioValida() || !desti.posicioValida())
        return false;

    Fitxa& fitxaOrigen = m_tauler[origen.getFila()][origen.getColumna()];
    Fitxa& fitxaDesti = m_tauler[desti.getFila()][desti.getColumna()];

    int nMoviments = fitxaOrigen.getNumMoviments();
    for (int i = 0; i < nMoviments; i++)
    {
        Moviment mov = fitxaOrigen.getMoviment(i);
        if (mov.getUltimaPosicio() == desti)
        {
            establirFitxa(desti, fitxaOrigen);
            establirFitxa(origen, Fitxa()); 

            if (esCaptura(origen, desti))
            {
                int filaDelMig = (origen.getFila() + desti.getFila()) / 2;
                int columnaDelMig = (origen.getColumna() + desti.getColumna()) / 2;
                establirFitxa(Posicio(filaDelMig, columnaDelMig), Fitxa());
            }

            movimentValid = true;
            break;
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

void Tauler::establirFitxa(const Posicio& posicio, const Fitxa& fitxa)
{
    if (posicio.posicioValida())
    {
        m_tauler[posicio.getFila()][posicio.getColumna()] = fitxa;
    }
}


string Tauler::toString() const
{
	string resultat = "";

    for (int fila = 0; fila < N_FILES; fila++)
    {
		for (int columna = 0; columna < N_COLUMNES; columna++)
		{
			Fitxa fitxa = m_tauler[fila][columna];

            if (!fitxa.esBuida())
            {
                char tipus;

                if (fitxa.getTipus() == TIPUS_DAMA)
                {
                    if (fitxa.getColor() == COLOR_BLANC)
                    {
                        tipus = 'D';
                    }
                    else
                    {
                        tipus = 'R';
                    }
                }
                else
                {
                    if (fitxa.getColor() == COLOR_BLANC)
                    {
                        tipus = 'O';
                    }
                    else
                    {
                        tipus = 'X';
                    }
                }
                
                char columna = 'zzzz';

                if (columna == 0)
                {
                    columna = 'a';
                }
                else if (columna == 1)
                {
                    columna = 'b';
                }
                else if (columna == 2)
                {
                    columna = 'c';
                }
                else if (columna == 3)
                {
                    columna = 'd';
                }
				else if (columna == 4)
				{
					columna = 'e';
				}
				else if (columna == 5)
				{
					columna = 'f';
				}
				else if (columna == 6)
				{
					columna = 'g';
				}
                else if (columna == 7)
                {
                    columna = 'h';
                }

				int numeroFila = N_FILES - fila;

                resultat = resultat + tipus;
                resultat = resultat + " ";
                resultat = resultat + columna;
				resultat += '0' + numeroFila;
				resultat = resultat + "\n";
            }
		}
    }
    return resultat;
}

void Tauler::inicialitzaTaulerVuit()
{
    for (int fila = 0; fila < N_FILES; fila++)
    {
        for (int columna = 0; columna < N_COLUMNES; columna++)
        {
            m_tauler[fila][columna] = Fitxa(); 
        }
    }
}

void Tauler::carregaFitxes(const string& nomFitxer)
{
    ifstream fitxer(nomFitxer);

    if (fitxer.is_open())
    {
        string tipus;
		string posicio;

        inicialitzaTaulerVuit();

        while (fitxer >> tipus >> posicio)
        {
            Fitxa novaFitxa;

			if (tipus == "O")
			{
				novaFitxa.setTipus(TIPUS_NORMAL);
				novaFitxa.setColor(COLOR_BLANC);
			}
			else if (tipus == "X")
			{
				novaFitxa.setTipus(TIPUS_NORMAL);
				novaFitxa.setColor(COLOR_NEGRE);
			}
			else if (tipus == "D")
			{
				novaFitxa.setTipus(TIPUS_DAMA);
				novaFitxa.setColor(COLOR_BLANC);
			}
			else if (tipus == "R")
			{
				novaFitxa.setTipus(TIPUS_DAMA);
				novaFitxa.setColor(COLOR_NEGRE);
			}

            char lletraColumna = posicio[0];
            int numeroFila = posicio[1] - '0';

            int fila = N_FILES - numeroFila;
			int columna = lletraColumna - 'a';

			m_tauler[fila][columna] = novaFitxa;
        }
    }

	fitxer.close();
}

void Tauler::obtenirPosicionsValides(const Posicio& origen, Posicio posicionsValides[], int& nPosicionsValides) 
{
	nPosicionsValides = 0;

	int fila = origen.getFila(); //coordenades
	int columna = origen.getColumna();

	Fitxa& fitxa = m_tauler[fila][columna];

    if (fitxa.getTipus() == TIPUS_DAMA)
    {
		bool esDama = true;

        if (fitxa.getColor() == COLOR_BLANC)
        {
			bool esBlanca = true;

            if (!esDama)
            {
                if (esBlanca)
                {
					if (fila + 1 < N_FILES && columna + 1 < N_COLUMNES && m_tauler[fila + 1][columna + 1].esBuida()) //diagonal dreta blanca
					{
                        posicionsValides[nPosicionsValides++] = Posicio(fila + 1, columna + 1);
					}

                    if (fila + 1 < N_FILES && columna - 1 >= 0 && m_tauler[fila + 1][columna - 1].esBuida()) //diagonal esquerra blanca
                    {
                        posicionsValides[nPosicionsValides++] = Posicio(fila + 1, columna - 1);
                    }
                }
                else
                {
					if (fila - 1 >= 0 && columna + 1 < N_COLUMNES && m_tauler[fila - 1][columna + 1].esBuida()) //diagonal dreta negra
                    {
						posicionsValides[nPosicionsValides++] = Posicio(fila - 1, columna + 1);
                    }

					if (fila - 1 >= 0 && columna - 1 >= 0 && m_tauler[fila - 1][columna - 1].esBuida()) //diagonal esquerra negra
                    {
						posicionsValides[nPosicionsValides++] = Posicio(fila - 1, columna - 1);
                    }
                }
            }
            else
            {
                if (fila + 1 < N_FILES && columna + 1 < N_COLUMNES && m_tauler[fila + 1][columna + 1].esBuida()) //diagonal dreta dama endavant
                {
                    posicionsValides[nPosicionsValides++] = Posicio(fila + 1, columna + 1);
                }

                if (fila + 1 < N_FILES && columna - 1 >= 0 && m_tauler[fila + 1][columna - 1].esBuida()) //diagonal esquerra dama endavant
                {
                    posicionsValides[nPosicionsValides++] = Posicio(fila + 1, columna - 1);
                }

				if (fila - 1 >= 0 && columna + 1 < N_COLUMNES && m_tauler[fila - 1][columna + 1].esBuida()) //diagonal dreta dama endarere
                {
                    posicionsValides[nPosicionsValides++] = Posicio(fila - 1, columna + 1);
                }

                if (fila - 1 >= 0 && columna - 1 >= 0 && m_tauler[fila - 1][columna - 1].esBuida()) //diagonal esquerra dama endarere
                {
                    posicionsValides[nPosicionsValides++] = Posicio(fila - 1, columna - 1);
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

	if ((filaOrigen - filaDesti) == 2 || (filaOrigen - filaDesti) == -2)
	{
		if ((filaOrigen - filaDesti) == 2 || (filaOrigen - filaDesti) == -2 && (columnaOrigen - columnaDesti) == 2 || (columnaOrigen - columnaDesti) == -2)
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
	}

    return capturem;
}
