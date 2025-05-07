#include "tauler.hpp"
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

			switch (tipus) // assignem segons el tipus i color de fitxa
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
                getPosicionsPossibles(origen, nPosicions, posicionsPossibles); // posicions possibles la fitxa pot moure

                for (int k = 0; k < nPosicions; k++)
                {
                    Moviment moviment;
                    moviment.afegeixPosicio(origen);
                    moviment.afegeixPosicio(posicionsPossibles[k]); //destinacio

                    if (esCaptura(origen, posicionsPossibles[k]))
                    {
                        moviment.setPecesMates(1); //marquem
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

                for (int k = 0; k < nMoviments; k++) //obtenim nomes moviments amb captura
                {
                    if (fitxa.getMoviment(k).getPecesMates() > 0)
                    {
                        nousMoviments[comptador++] = fitxa.getMoviment(k);
                    }
                }

				fitxa.netejaMoviments(); //netegem els moviments anteriors dels nous

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

	Fitxa fitxaOrigen = m_tauler[origen.getFila()][origen.getColumna()];  //obtenim la pos fitxa origen

	for (int i = 0; i < fitxaOrigen.getNumMoviments(); i++) //recorrem els moviments possibles
    {
        Moviment mov = fitxaOrigen.getMoviment(i);

        if (mov.getUltimaPosicio() == desti) 
        {
            movimentValid = true;

			m_tauler[origen.getFila()][origen.getColumna()] = Fitxa(); //netegem la fitxa origen

			m_tauler[desti.getFila()][desti.getColumna()] = fitxaOrigen; //assignem la fitxa al desti

            if (mov.getPecesMates() > 0) 
            {
                for (int j = 1; j < mov.getNumPosicions(); j++) //parelles de moviments
                {
                    Posicio p1 = mov.getPosicio(j - 1); //anterior
                    Posicio p2 = mov.getPosicio(j); //actual

                    int filaOrigen = p1.getFila();
                    int columnaOrigen = p1.getColumna();
                    int filaDesti = p2.getFila();
                    int columnaDesti = p2.getColumna();

                    // ens volem moure diagonalment
                    int incrementFila = 0;
                    if (filaDesti > filaOrigen)
                    {
                        incrementFila = 1;
                    }
                    else if (filaDesti < filaOrigen)
                    {
                        incrementFila = -1;
                    }

                    int incrementColumna = 0;
                    if (columnaDesti > columnaOrigen)
                    {
                        incrementColumna = 1;
                    }
                    else if (columnaDesti < columnaOrigen)
                    {
                        incrementColumna = -1;
                    }

                    int filaActual = filaOrigen + incrementFila;
                    int columnaActual = columnaOrigen + incrementColumna;

					while (filaActual != filaDesti || columnaActual != columnaDesti) // mirem si hi ha fitxes intermedies
                    {
                        Fitxa& fitxaIntermedia = m_tauler[filaActual][columnaActual];
                        if (!fitxaIntermedia.esBuida() && fitxaIntermedia.getColor() != fitxaOrigen.getColor()) //vol dir fitxa enemiga
                        {
							m_tauler[filaActual][columnaActual] = Fitxa(); //netegem la fitxa enemiga
                        }
                        filaActual += incrementFila;
                        columnaActual += incrementColumna;
                    }
                }
            }

			//dama
            if ((fitxaOrigen.getColor() == COLOR_BLANC && desti.getFila() == 0) ||
                (fitxaOrigen.getColor() == COLOR_NEGRE && desti.getFila() == N_FILES - 1))
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
		Moviment movimentActual = movimentsPendents[0]; // agafem el primer moviment pendent

        for (int i = 0; i < numMovimentsPendents - 1; i++) //desplacem els moviments
        {
            movimentsPendents[i] = movimentsPendents[i + 1];
        }
        numMovimentsPendents--;

        Posicio posicioActual = movimentActual.getUltimaPosicio();

        Posicio posicionsValides[MAX_POSICIONS];
        int nPosicionsValides = 0;
        obtenirPosicionsValides(posicioActual, posicionsValides, nPosicionsValides);

        bool haTrobatCaptura = false;

        for (int j = 0; j < nPosicionsValides; j++) //recorrem les posicions valides
        {
            Posicio novaPosicio = posicionsValides[j];

			Moviment nouMoviment = movimentActual; //copiem el moviment actual i posem el nou
            nouMoviment.afegeixPosicio(novaPosicio);

            if (esCaptura(posicioActual, novaPosicio))
            {
                haTrobatCaptura = true;
                movimentsPendents[numMovimentsPendents++] = nouMoviment;// Afegim el nou moviment a la cua per continuar buscant captures encadenades
            }
            else
            {
                posicionsPossibles[nPosicions++] = novaPosicio; 
            }
        }
		// si no hi ha captures
        if (nPosicionsValides == 0 && movimentActual.getNumPosicions() > 1)
        {
            posicionsPossibles[nPosicions++] = posicioActual;
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
		char numeroFila = '0' + (N_FILES - fila); // convertim a char el numero de fila
        resultat = resultat + numeroFila + ": ";

        for (int columna = 0; columna < N_COLUMNES; columna++)
        {
            char c = '_'; //defecte casella buida
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

			// Recorrem la diagonal fins que trobem una fitxa o sortim del tauler
            while (!movimentFinalitzat && filaActual >= 0 && filaActual < N_FILES && columnaActual >= 0 && columnaActual < N_COLUMNES)
            {
                if (m_tauler[filaActual][columnaActual].esBuida())
                {
                    posicionsValides[nPosicionsValides++] = Posicio(filaActual, columnaActual);
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

		for (int movimentHoritzontal = -1; movimentHoritzontal <= 1; movimentHoritzontal += 2) // comprovem si es pot moure a l'esquerra o a la dreta
        {
            int filaDesti = filaOrigen + movimentVertical;
            int columnaDesti = columnaOrigen + movimentHoritzontal;

            if (filaDesti >= 0 && filaDesti < N_FILES && columnaDesti >= 0 && columnaDesti < N_COLUMNES)
            {
                if (m_tauler[filaDesti][columnaDesti].esBuida())
                {
                    posicionsValides[nPosicionsValides++] = Posicio(filaDesti, columnaDesti); // moviment valid
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
	bool capturaValida = false;
    int filaOrigen = origen.getFila();
    int columnaOrigen = origen.getColumna();
    int filaDesti = desti.getFila();
    int columnaDesti = desti.getColumna();

    const Fitxa& fitxaOrigen = m_tauler[filaOrigen][columnaOrigen];

    if (fitxaOrigen.getTipus() == TIPUS_NORMAL)
    {
        if (abs(filaOrigen - filaDesti) == 2 && abs(columnaOrigen - columnaDesti) == 2)
        {
			// Comprovem si hi ha una fitxa enemiga entre origen i desti
            int filaDelMig = (filaOrigen + filaDesti) / 2;
            int columnaDelMig = (columnaOrigen + columnaDesti) / 2;

            const Fitxa& fitxaDelMig = m_tauler[filaDelMig][columnaDelMig];
            if (!fitxaDelMig.esBuida() && fitxaDelMig.getColor() != fitxaOrigen.getColor())
            {
                capturaValida = true;
            }
        }
    }
    else if (fitxaOrigen.getTipus() == TIPUS_DAMA)
    {
		// Comprovem si el moviment es diagonal i si hi ha una fitxa enemiga entre origen i desti
        int incrementFila = 0;
        if (filaDesti > filaOrigen)
        {
            incrementFila = 1;
        }
        else if (filaDesti < filaOrigen)
        {
            incrementFila = -1;
        }

        int incrementColumna = 0;
        if (columnaDesti > columnaOrigen)
        {
            incrementColumna = 1;
        }
        else if (columnaDesti < columnaOrigen)
        {
            incrementColumna = -1;
        }

        int filaActual = filaOrigen + incrementFila;
        int columnaActual = columnaOrigen + incrementColumna;

        int comptadorEnemics = 0;
	
		while (filaActual != filaDesti && columnaActual != columnaDesti) // recorrem la diagonal
        {
            const Fitxa& fitxaActual = m_tauler[filaActual][columnaActual];

            if (!fitxaActual.esBuida())
            {
                if (fitxaActual.getColor() != fitxaOrigen.getColor())
                {
                    comptadorEnemics++;
                }
                else
                {
                    capturaValida = false;
                }
            }
            filaActual += incrementFila;
            columnaActual += incrementColumna;
        }

        if (comptadorEnemics == 1)
        {
            capturaValida = true;
        }
    }

    return capturaValida;
}