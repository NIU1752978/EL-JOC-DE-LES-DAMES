#include "Tauler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Fitxa.h"
#include "info_joc.hpp"
#include <vector>
#include <algorithm>
#include "GraphicManager.h"

using namespace std;

Tauler::Tauler() {
    inicialitzaTaulerBuit();  // Inicialitza el tauler amb fitxes buides
}

Tauler::~Tauler() {
    // Allibera la memòria de totes les fitxes del tauler
    for (int i = 0; i < N_FILES; ++i) {
        for (int j = 0; j < N_COLUMNES; ++j) {
            delete m_tauler[i][j];
            m_tauler[i][j] = nullptr;
        }
    }
}

void Tauler::inicialitzaTaulerBuit() {
    // Assigna una fitxa buida a cada casella del tauler
    for (int i = 0; i < N_FILES; ++i) {
        for (int j = 0; j < N_COLUMNES; ++j) {
            delete m_tauler[i][j];
            m_tauler[i][j] = new Fitxa(); // fitxa buida
        }
    }
}

void Tauler::inicialitza(const std::string& nomFitxer)
{
    inicialitzaTaulerBuit(); // buida el tauler abans de carregar

    std::ifstream fitxer(nomFitxer);
    if (fitxer.is_open())
    {
        char tipusChar;
        std::string posicioStr;

        while (fitxer >> tipusChar >> posicioStr) // per cada línia
        {
            TipusFitxa tipusFitxa;
            ColorFitxa colorFitxa;
            bool valid = true;

            // Tradueix el caràcter del fitxer a tipus i color de fitxa
            switch (tipusChar)
            {
            case 'O': tipusFitxa = TIPUS_NORMAL; colorFitxa = COLOR_BLANC; break;
            case 'X': tipusFitxa = TIPUS_NORMAL; colorFitxa = COLOR_NEGRE; break;
            case 'D': tipusFitxa = TIPUS_DAMA;   colorFitxa = COLOR_BLANC; break;
            case 'R': tipusFitxa = TIPUS_DAMA;   colorFitxa = COLOR_NEGRE; break;
            default: valid = false; break;
            }

			if (valid) // només si el tipus i color són vàlids
            {
                Posicio pos(posicioStr);
                int fila = pos.getFila();
                int col = pos.getColumna();

				if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) // comprova que la posició és vàlida
                {
					Fitxa* fitxa = new Fitxa(tipusFitxa, colorFitxa); // crea la fitxa amb el tipus i color corresponents
					delete m_tauler[fila][col]; // allibera la fitxa existent a la posició
					m_tauler[fila][col] = fitxa; // assigna la nova fitxa a la posició del tauler
                }
            }
        }

        fitxer.close();
    }
}


void Tauler::establirFitxa(const Posicio& pos, Fitxa* fitxa)
{
	int fila = pos.getFila(); // Obtenim la fila i columna de la posició
	int col = pos.getColumna(); // Obtenim la columna de la posició
	delete m_tauler[fila][col]; // Alliberem la fitxa existent a la posició
	m_tauler[fila][col] = fitxa; // Assignem la nova fitxa a la posició del tauler
}

void Tauler::actualitzaMovimentsValids()
{
	for (int fila = 0; fila < N_FILES; ++fila)
    {
        for (int columna = 0; columna < N_COLUMNES; ++columna)
        {
			Fitxa& fitxa = *m_tauler[fila][columna]; // Obtenim la fitxa a la posició actual

			if (!fitxa.esBuida()) // Si la fitxa no és buida
            {
                fitxa.netejaMoviments();

				if (fitxa.getTipus() == TIPUS_NORMAL) // Si és una fitxa normal
                {
                    afegeixMovimentsFitxaNormal(fila, columna);
                }
				else if (fitxa.getTipus() == TIPUS_DAMA) // Si és una dama
                {
                    afegeixMovimentsFitxaDama(fila, columna);
                }
            }
        }
    }
}

Moviment Tauler::mouFitxa(const Posicio& origen, const Posicio& desti, ColorFitxa tornActual)
{
    Moviment resultat;

    int filaOri = origen.getFila();
    int colOri = origen.getColumna();
    int filaDes = desti.getFila();
    int colDes = desti.getColumna();

    bool movimentValid = false;
    Moviment movimentSeleccionat;
     
	if (esPosicioDinsTauler(filaOri, colOri) && esPosicioDinsTauler(filaDes, colDes)) { // Comprova que les posicions són vàlides
        Fitxa* fitxa = m_tauler[filaOri][colOri];

		if (!fitxa->esBuida() && fitxa->getColor() == tornActual) { // Comprova que la fitxa no és buida i que és del color del torn actual
			for (int i = 0; i < fitxa->getNumMoviments(); ++i) { // Itera sobre els moviments de la fitxa
                Moviment m = fitxa->getMoviment(i);
				if (m.getDesti() == desti) { // Si el moviment té la mateixa destinació que la sol·licitada
                    movimentSeleccionat = m;
                    movimentValid = true;
                }
            }

			if (movimentValid) { // Si s'ha trobat un moviment vàlid
                int maxPosicions = 0;
				for (int i = 0; i < fitxa->getNumMoviments(); ++i) { // Itera sobre els moviments de la fitxa per trobar el màxim de posicions
                    Moviment m = fitxa->getMoviment(i);
					if (m.getNPosicions() >= 2) { // Només considerem moviments amb més d'una posició
                        const Posicio& p1 = m.getPosicio(0);
                        const Posicio& p2 = m.getPosicio(1);
						if (abs(p1.getFila() - p2.getFila()) > 1 && abs(p1.getColumna() - p2.getColumna()) > 1) { // Comprova si el moviment és una captura real
                            maxPosicions = std::max(maxPosicions, m.getNPosicions());
                        }
                    }
                }

                int posicionsFetes = movimentSeleccionat.getNPosicions();
                bool esCaptura = esMovimentCapturaReal(movimentSeleccionat);

                bool movimentAcceptat = true;

				if (esCaptura && posicionsFetes < maxPosicions) { // Si és una captura però no s'ha fet el màxim de captures possibles
                    eliminaFitxesCapturades(movimentSeleccionat);
                    delete m_tauler[filaOri][colOri];
                    m_tauler[filaOri][colOri] = new Fitxa();
                    movimentAcceptat = false;
                }

				if (!esCaptura) { // Si no és una captura, comprovem si hi ha captures disponibles al tauler
                    bool algunaCapturaAlTauler = false;

                    for (int fila = 0; fila < N_FILES && !algunaCapturaAlTauler; ++fila) {
                        for (int col = 0; col < N_COLUMNES && !algunaCapturaAlTauler; ++col) {
                            Fitxa* fitxaTemp = m_tauler[fila][col];
							if (!fitxaTemp->esBuida() && fitxaTemp->getColor() == tornActual) { // Si la fitxa és del color del torn actual
								for (int i = 0; i < fitxaTemp->getNumMoviments(); ++i) { // Itera sobre els moviments de la fitxa
									if (esMovimentCapturaReal(fitxaTemp->getMoviment(i))) { // Si el moviment és una captura real
                                        algunaCapturaAlTauler = true;
                                    }
                                }
                            }
                        }
                    }

					if (algunaCapturaAlTauler) { // Si hi ha captures disponibles al tauler
                        delete m_tauler[filaOri][colOri];
                        m_tauler[filaOri][colOri] = new Fitxa();
                        movimentAcceptat = false;
                    }
                }

				if (movimentAcceptat) { // Si el moviment és vàlid i acceptat
                    eliminaFitxesCapturades(movimentSeleccionat);
                    delete m_tauler[filaDes][colDes];
                    m_tauler[filaDes][colDes] = fitxa;
                    m_tauler[filaOri][colOri] = new Fitxa();

                    if ((fitxa->getColor() == COLOR_BLANC && filaDes == 0) ||
						(fitxa->getColor() == COLOR_NEGRE && filaDes == N_FILES - 1)) { // Si la fitxa arriba a l'altre extrem del tauler
                        fitxa->convertirEnDama();
                    }

                    actualitzaMovimentsValids();
                }

                resultat = movimentSeleccionat;
            }
        }
    }

    return resultat;
}

void Tauler::getPosicionsPossibles(const Posicio& origen, std::vector<Posicio>& posicionsPossibles) const {
    posicionsPossibles.clear();

    int fila = origen.getFila();
    int columna = origen.getColumna();
    const Fitxa& fitxa = *m_tauler[fila][columna];

    std::vector<Posicio> destins; 
    std::vector<int> captures; 
    std::vector<int> dames;

	for (int i = 0; i < fitxa.getNumMoviments(); ++i) { // Itera sobre els moviments de la fitxa
        Moviment m = fitxa.getMoviment(i);
		if (m.getNPosicions() > 0) { // Si el moviment té posicions
            Posicio desti = m.getDesti();

            bool mateixaPosicio = (desti.getFila() == origen.getFila()) && (desti.getColumna() == origen.getColumna());

			if (!mateixaPosicio) { // Si la posició de destí no és la mateixa que l'origen
                bool jaExisteix = false;
                size_t j = 0;
				while (j < destins.size()) { // Comprova si ja existeix la posició de destí
                    bool mateixaFila = (destins[j].getFila() == desti.getFila());
                    bool mateixaColumna = (destins[j].getColumna() == desti.getColumna());
					if (mateixaFila && mateixaColumna) { // Si la posició ja existeix
                        jaExisteix = true;
                        j = destins.size(); // surt del while
                    }
                    ++j;
                }

                if (!jaExisteix) {
                    destins.push_back(desti);
                    captures.push_back(m.getNPosicions() - 1);
                    dames.push_back(m.getNumDamesCapturades(*this));
                }
            }
        }
    }

    // Ordenació mitjançant vector d'índexs
    std::vector<int> indexos(destins.size());
	for (size_t i = 0; i < indexos.size(); ++i) indexos[i] = static_cast<int>(i); // Inicialitzem l'índexs amb posicions 0, 1, 2, ...

    
	for (size_t i = 0; i < indexos.size(); ++i) {  // Ordenem els índexs segons captures i dames
		for (size_t j = i + 1; j < indexos.size(); ++j) { // Compara cada parell d'índexs
            bool millor = false;
			if (captures[indexos[j]] > captures[indexos[i]]) { // Si la captura del segon índex és millor
                millor = true;
            }
			else if (captures[indexos[j]] == captures[indexos[i]]) { // Si les captures són iguals, comparem les dames
				if (dames[indexos[j]] > dames[indexos[i]]) { // Si la dama del segon índex és millor
                    millor = true;
                }
            }

			if (millor) { // Si el segon índex és millor, els intercanviem
                int temp = indexos[i];
                indexos[i] = indexos[j];
                indexos[j] = temp;
            }
        }
    }

    // Afegim les posicions finals ordenades
	for (size_t i = 0; i < indexos.size(); ++i) { // Iterem sobre els índexs ordenats
        posicionsPossibles.push_back(destins[indexos[i]]);
    }
}

std::string Tauler::toString() const {
    std::ostringstream resultat;

    for (int fila = 0; fila < N_FILES; fila++) { // Itera sobre les files del tauler
        char numeroFila = '0' + (N_FILES - fila);
        resultat << numeroFila << ": ";

        for (int columna = 0; columna < N_COLUMNES; columna++) { // Itera sobre les columnes del tauler
            char c = '_';
            const Fitxa& fitxa = *m_tauler[fila][columna];

            if (!fitxa.esBuida()) {
                if (fitxa.getTipus() == TIPUS_DAMA) {
                    if (fitxa.getColor() == COLOR_BLANC) {
                        c = 'D';
                    }
                    else {
                        c = 'R';
                    }
                }
                else {
                    if (fitxa.getColor() == COLOR_BLANC) {
                        c = 'O';
                    }
                    else {
                        c = 'X';
                    }
                }
            }

            resultat << c << ' ';
        }

        resultat << '\n';
    }

    resultat << "   a b c d e f g h\n";
    return resultat.str();
}

void Tauler::afegeixMovimentsFitxaNormal(int fila, int columna) {
    // Obtenim una referència a la fitxa situada a la posició indicada
    Fitxa& fitxa = *m_tauler[fila][columna];

    // Verifiquem que la fitxa és de tipus normal i que no està buida
    bool esFitxaNormal = (fitxa.getTipus() == TIPUS_NORMAL);
    bool teFitxa = !fitxa.esBuida();

    if (esFitxaNormal && teFitxa) {
        // Inicialitzem estructura per guardar possibles captures
        Moviment capturesPossibles[MAX_MOVIMENTS];
        int nombreCaptures = 0;
        bool sHaTrobatCaptura = false;

        // Creem el moviment inicial amb la posició actual
        Moviment movimentInici;
        movimentInici.afegirPosicio(Posicio(fila, columna));

        // Omplim l'array de captures possibles a partir de la posició actual
        obtenirMovimentsDesDePosicio(
            fitxa,
            fila,
            columna,
            movimentInici,
            capturesPossibles,
            nombreCaptures,
            sHaTrobatCaptura
        );

        // Afegim al conjunt de moviments vàlids les captures trobades
        for (int i = 0; i < nombreCaptures; i++) {
            fitxa.afegeixMovimentValid(capturesPossibles[i]);
        }

        // Si no s'ha trobat cap captura, afegim moviments normals (una casella endavant en diagonal)
        if (!sHaTrobatCaptura) {
            // Determinem la direcció segons el color de la fitxa
            int direccio = 0;
            if (fitxa.getColor() == COLOR_BLANC)
                direccio = -1;
            else if (fitxa.getColor() == COLOR_NEGRE)
                direccio = 1;

            // Recorrem les dues diagonals (esquerra i dreta)
            for (int desviamentColumna = -1; desviamentColumna <= 1; desviamentColumna += 2) {
                int novaFila = fila + direccio;
                int novaColumna = columna + desviamentColumna;

                // Comprovem si la nova posició està dins del tauler i buida
                bool dinsLimits = esPosicioDinsTauler(novaFila, novaColumna);
                bool casellaBuida = false;

                if (dinsLimits)
                    casellaBuida = m_tauler[novaFila][novaColumna]->esBuida();

                if (dinsLimits && casellaBuida) {
                    // Creem i afegim el moviment normal
                    Moviment m;
                    m.afegirPosicio(Posicio(fila, columna));
                    m.afegirPosicio(Posicio(novaFila, novaColumna));
                    fitxa.afegeixMovimentValid(m);
                }
            }
        }
    }
}

void Tauler::afegeixMovimentsFitxaDama(int fila, int columna) {
    // Obtenim la fitxa que volem analitzar
    Fitxa& fitxa = *m_tauler[fila][columna];

    // Esborrem qualsevol moviment que hi hagués prèviament a la fitxa
    fitxa.netejaMoviments();

    // Creem un moviment inicial que comença des de la posició actual
    Moviment movimentInicial;
    movimentInicial.afegirPosicio(Posicio(fila, columna));

    // Preparem l'estructura per guardar tots els moviments possibles
    Moviment movimentsDetectats[MAX_MOVIMENTS];
    int quantitatMoviments = 0;

    // Cerquem totes les opcions de moviment possibles per a aquesta dama
    obtenirMovimentsDamaDesDePosicio(
        fitxa,
        fila,
        columna,
        movimentInicial,
        movimentsDetectats,
        quantitatMoviments
    );

    // Afegim al conjunt de moviments vàlids els que s'han trobat
    for (int i = 0; i < quantitatMoviments; i++) {
        fitxa.afegeixMovimentValid(movimentsDetectats[i]);
    }
}

bool Tauler::esCapturaDisponible(const Fitxa& fitxa, int fila, int columna) const {
    bool capturaTrobada = false;

    // Comprovem que la fitxa no estigui buida
    if (!fitxa.esBuida()) {
        ColorFitxa colorJugador = fitxa.getColor();
        TipusFitxa tipusFitxa = fitxa.getTipus();

        // Direccions diagonals: dreta avall, esquerra avall, dreta amunt, esquerra amunt
        const int direccions[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

        // Recorrem totes les direccions mentre no hàgim trobat cap captura
        for (int i = 0; i < 4 && !capturaTrobada; i++) {
            int deltaFila = direccions[i][0];
            int deltaColumna = direccions[i][1];

            // Tractament per a fitxes normals
            if (tipusFitxa == TIPUS_NORMAL) {
                int direccioValida = 0;
                if (colorJugador == COLOR_BLANC)
                    direccioValida = -1;
                else if (colorJugador == COLOR_NEGRE)
                    direccioValida = 1;

                // Només comprovem direccions vàlides segons el color
                if (deltaFila == direccioValida) {
                    int filaMig = fila + deltaFila;
                    int colMig = columna + deltaColumna;
                    int filaDesti = fila + 2 * deltaFila;
                    int colDesti = columna + 2 * deltaColumna;

                    if (esPosicioDinsTauler(filaMig, colMig) && esPosicioDinsTauler(filaDesti, colDesti)) {
                        const Fitxa& fitxaMig = *m_tauler[filaMig][colMig];
                        const Fitxa& fitxaDesti = *m_tauler[filaDesti][colDesti];

                        bool hiHaFitxaContraria = !fitxaMig.esBuida() && fitxaMig.getColor() != colorJugador;
                        bool casellaDestiBuida = fitxaDesti.esBuida();

                        if (hiHaFitxaContraria && casellaDestiBuida) {
                            capturaTrobada = true;
                        }
                    }
                }
            }

            // Tractament per a dames
            else if (tipusFitxa == TIPUS_DAMA) {
                int filaActual = fila + deltaFila;
                int colActual = columna + deltaColumna;
                bool haTrobatContrari = false;

                // Recorrem la diagonal fins trobar una captura o una obstrucció
                while (esPosicioDinsTauler(filaActual, colActual) && !capturaTrobada) {
                    const Fitxa& fitxaActual = *m_tauler[filaActual][colActual];

                    if (fitxaActual.esBuida()) {
                        // Si ja hem passat una fitxa contrària, i ara tenim casella buida, podem capturar
                        if (haTrobatContrari) {
                            capturaTrobada = true;
                        }

                        // Continuem avançant per la diagonal
                        filaActual += deltaFila;
                        colActual += deltaColumna;
                    }
                    else {
                        bool esMateixColor = (fitxaActual.getColor() == colorJugador);

                        // Si trobem fitxa pròpia o més d’una contrària seguida, no és vàlid
                        if (esMateixColor || haTrobatContrari) {
                            // Sortim del bucle posant límits fora del tauler
                            filaActual = N_FILES;
                            colActual = N_COLUMNES;
                        }
                        else {
                            // Primera fitxa contrària trobada
                            haTrobatContrari = true;
                            filaActual += deltaFila;
                            colActual += deltaColumna;
                        }
                    }
                }
            }
        }
    }

    return capturaTrobada;
}

bool Tauler::esPosicioDinsTauler(int fila, int columna) const {
    return (fila >= 0 && fila < N_FILES && columna >= 0 && columna < N_COLUMNES);
}

bool Tauler::hiHaFitxaContraria(int fila, int col, ColorFitxa colorPropi) const
{
    // Inicialitzem la variable resultat com a fals (no s'ha trobat fitxa contrària)
    bool resultat = false;

    // Comprovem que la posició està dins dels límits del tauler
    if (esPosicioDinsTauler(fila, col)) {
        // Obtenim la fitxa a la posició especificada
        const Fitxa& fitxa = *m_tauler[fila][col];

        // Si la casella no està buida
        if (!fitxa.esBuida()) {
            // Comprovem si el color de la fitxa és diferent del del jugador
            if (fitxa.getColor() != colorPropi) {
                resultat = true; // És una fitxa contrària
            }
        }
    }

    return resultat;
}

bool Tauler::esCapturaPossible(const Fitxa& fitxa, int fila, int columna) const {
    // Inicialment suposem que no hi ha cap captura possible
    bool capturaPossible = false;

    // Comprovem que la posició és vàlida i la fitxa no està buida
    if (!fitxa.esBuida() && esPosicioDinsTauler(fila, columna)) {
        // Determinem la direcció de moviment en funció del color de la fitxa
        int direccio;
        if (fitxa.getColor() == COLOR_BLANC)
            direccio = -1; // El blanc es mou cap amunt
        else
            direccio = 1;  // El negre es mou cap avall

        // Recorrem les dues diagonals possibles: esquerra i dreta
        for (int desplacamentColumna = -1; desplacamentColumna <= 1; desplacamentColumna += 2) {
            int filaMig = fila + direccio;               // Fila de la fitxa contrària
            int columnaMig = columna + desplacamentColumna; // Columna de la fitxa contrària

            int filaDesti = fila + 2 * direccio;          // Casella buida de destí
            int columnaDesti = columna + 2 * desplacamentColumna;

            // Comprovem que tant la posició intermitja com la final són dins del tauler
            bool posicionsValides =
                esPosicioDinsTauler(filaMig, columnaMig) &&
                esPosicioDinsTauler(filaDesti, columnaDesti);

            if (posicionsValides) {
                const Fitxa& fitxaMig = *m_tauler[filaMig][columnaMig];
                const Fitxa& fitxaDesti = *m_tauler[filaDesti][columnaDesti];

                // Comprovem que hi hagi una fitxa contrària i una casella de destí buida
                bool hiHaFitxaContraria = !fitxaMig.esBuida() && fitxaMig.getColor() != fitxa.getColor();
                bool destiBuida = fitxaDesti.esBuida();

                if (hiHaFitxaContraria && destiBuida) {
                    capturaPossible = true;
                }
            }
        }
    }

    return capturaPossible;
}

void Tauler::simulaCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, Fitxa& fitxaCapturada, Fitxa& fitxaOriginal)
{
    // Guarda la fitxa capturada i la que es mourà
    fitxaCapturada = *m_tauler[mig.getFila()][mig.getColumna()];
    fitxaOriginal = *m_tauler[origen.getFila()][origen.getColumna()];

    // Buida la casella origen i la capturada
    *m_tauler[origen.getFila()][origen.getColumna()] = Fitxa(); // Buit
    *m_tauler[mig.getFila()][mig.getColumna()] = Fitxa();       // Buit

    // Mou la fitxa original a destí
    *m_tauler[desti.getFila()][desti.getColumna()] = fitxaOriginal;
}

void Tauler::desfesCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, const Fitxa& fitxaCapturada, const Fitxa& fitxaOriginal) {
    // Tornem la peça original a la seva posició
    *m_tauler[origen.getFila()][origen.getColumna()] = fitxaOriginal;

    // Tornem la peça capturada al seu lloc
    *m_tauler[mig.getFila()][mig.getColumna()] = fitxaCapturada;

    // Eliminem la peça que havia anat a destí
    *m_tauler[desti.getFila()][desti.getColumna()] = Fitxa();
}

void Tauler::obtenirMovimentsDesDePosicio(
const Fitxa& fitxa,
    int fila,
    int columna,
    Moviment movimentActual,
    Moviment possiblesMoviments[],
    int& nMoviments,
    bool& capturaTrobada)
{
    // Direccions diagonals: baix-dreta, baix-esquerra, amunt-dreta, amunt-esquerra
    const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    bool haTrobatCaptura = false;

    // Determinem la direcció de moviment segons el color
    int direccio;
    if (fitxa.getColor() == COLOR_BLANC)
        direccio = -1;
    else
        direccio = 1;

    // Recorrem totes les diagonals
    for (int i = 0; i < 4; i++) {
        int dr = dirs[i][0];
        int dc = dirs[i][1];

        // Les fitxes normals només poden capturar cap endavant
        if (dr == direccio) {
            int filaMig = fila + dr;
            int colMig = columna + dc;
            int filaDest = fila + 2 * dr;
            int colDest = columna + 2 * dc;

            // Comprovem si les posicions estan dins del tauler
            bool dinsTauler = esPosicioDinsTauler(filaMig, colMig) &&
                esPosicioDinsTauler(filaDest, colDest);

            if (dinsTauler) {
                bool esContraria = hiHaFitxaContraria(filaMig, colMig, fitxa.getColor());
                bool destiBuida = m_tauler[filaDest][colDest]->esBuida();

                if (esContraria && destiBuida) {
                    // Hem trobat una captura
                    haTrobatCaptura = true;
                    capturaTrobada = true;

                    // Guardem estat abans de simular
                    Fitxa fitxaCapturada, fitxaOriginal;
                    Posicio origen(fila, columna);
                    Posicio mig(filaMig, colMig);
                    Posicio desti(filaDest, colDest);

                    simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                    // Afegim la nova posició al moviment actual
                    Moviment nouMoviment = movimentActual;
                    nouMoviment.afegirPosicio(desti);

                    // Recorrem recursivament per buscar captures múltiples
                    Fitxa& novaFitxa = *m_tauler[filaDest][colDest];
                    obtenirMovimentsDesDePosicio(
                        novaFitxa, filaDest, colDest,
                        nouMoviment, possiblesMoviments, nMoviments, capturaTrobada);

                    // Si hi ha més d'una posició en el moviment, el considerem vàlid
                    if (nouMoviment.getNPosicions() > 1 && nMoviments < MAX_MOVIMENTS) {
                        possiblesMoviments[nMoviments] = nouMoviment;
                        nMoviments++;
                    }

                    // Recuperem l'estat anterior del tauler
                    desfesCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);
                }
            }
        }
    }

    // Si no s'han trobat captures en cap direcció
    if (!haTrobatCaptura) {
        // Si el moviment actual té més d'una posició (per exemple, una captura incompleta), l'afegim
        if (movimentActual.getNPosicions() > 1) {
            if (nMoviments < MAX_MOVIMENTS) {
                possiblesMoviments[nMoviments] = movimentActual;
                nMoviments++;
            }
        }
        // Si no s'ha trobat cap captura en cap moment (ni abans ni ara), afegim moviments normals
        else if (!capturaTrobada) {
            int direccioMov = fitxa.getColor() == COLOR_BLANC ? -1 : 1;

            for (int desviament = -1; desviament <= 1; desviament += 2) {
                int novaFila = fila + direccioMov;
                int novaColumna = columna + desviament;

                bool dins = esPosicioDinsTauler(novaFila, novaColumna);
                bool buida = dins && m_tauler[novaFila][novaColumna]->esBuida();

                if (dins && buida) {
                    Moviment m;
                    m.afegirPosicio(Posicio(fila, columna));
                    m.afegirPosicio(Posicio(novaFila, novaColumna));

                    if (nMoviments < MAX_MOVIMENTS) {
                        possiblesMoviments[nMoviments] = m;
                        nMoviments++;
                    }
                }
            }
        }
    }
}

void Tauler::obtenirMovimentsDamaDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual, Moviment possiblesMoviments[], int& nMoviments) {
    // Definim les 4 direccions diagonals possibles
    const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    bool capturaTrobada = false; // Indica si s'ha trobat almenys una captura

    // Recorrem les 4 diagonals per buscar captures
    for (int d = 0; d < 4; ++d) {
        int dr = dirs[d][0];
        int dc = dirs[d][1];
        int f = fila + dr;
        int c = columna + dc;

        bool trobatContrari = false;           // S'indica si hem trobat una fitxa contrària
        int filaContrari = -1;
        int columnaContrari = -1;
        bool continuar = true;

        // Recorrem la diagonal mentre estiguem dins del tauler i no ens hàgim aturat
        while (esPosicioDinsTauler(f, c) && continuar) {
            if (m_tauler[f][c]->esBuida()) {
                // Si hem trobat una fitxa contrària abans i ara la casella és buida, es pot capturar
                if (trobatContrari) {
                    Posicio origen(fila, columna);
                    Posicio mig(filaContrari, columnaContrari);
                    Posicio desti(f, c);

                    Fitxa fitxaCapturada, fitxaOriginal;
                    // Simulem la captura de la fitxa contrària
                    simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                    Moviment nouMoviment = movimentActual;
                    nouMoviment.afegirPosicio(desti);

                    Fitxa novaFitxa = *m_tauler[f][c];
                    // Recursivament busquem més captures des de la nova posició
                    obtenirMovimentsDamaDesDePosicio(novaFitxa, f, c, nouMoviment, possiblesMoviments, nMoviments);

                    // Si s'ha fet una captura (mínim 2 posicions), l'afegim al vector
                    if (nouMoviment.getNPosicions() > 1) {
                        if (nMoviments < MAX_MOVIMENTS)
                            possiblesMoviments[nMoviments++] = nouMoviment;
                    }

                    // Restaurem l'estat del tauler després de la simulació
                    desfesCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);
                    capturaTrobada = true;
                }

                // Seguim mirant en la mateixa direcció
                f += dr;
                c += dc;
            }
            else {
                // Comprovem si la fitxa trobada és contrària
                bool esFitxaContraria = (m_tauler[f][c]->getColor() != fitxa.getColor());
                bool esPrimeraFitxa = false;

                if (!trobatContrari)
                    esPrimeraFitxa = true;

                // Si és la primera fitxa trobada i és contrària, marquem-la
                if (esPrimeraFitxa && esFitxaContraria) {
                    trobatContrari = true;
                    filaContrari = f;
                    columnaContrari = c;
                    f += dr;
                    c += dc;
                }
                else {
                    continuar = false; // No es pot capturar, sortim del bucle
                }
            }
        }
    }

    // Si el moviment només té la posició inicial (no ha capturat), afegim moviments normals
    if (movimentActual.getNPosicions() == 1) {
        for (int d = 0; d < 4; ++d) {
            int dr = dirs[d][0];
            int dc = dirs[d][1];
            int f = fila + dr;
            int c = columna + dc;
            bool continuar = true;

            // Recorrem la diagonal afegint moviments mentre les caselles estiguin buides
            while (esPosicioDinsTauler(f, c) && m_tauler[f][c]->esBuida() && continuar) {
                Moviment m;
                m.afegirPosicio(Posicio(fila, columna));
                m.afegirPosicio(Posicio(f, c));

                if (nMoviments < MAX_MOVIMENTS)
                    possiblesMoviments[nMoviments++] = m;

                f += dr;
                c += dc;
            }
        }
    }
}

void Tauler::eliminaFitxesCapturades(const Moviment& m) {
    // Recorrem cada tram del moviment (cada parell de posicions consecutives)
    for (int i = 0; i < m.getNPosicions() - 1; ++i) {
        Posicio p1 = m.getPosicio(i);       // Posició d'origen
        Posicio p2 = m.getPosicio(i + 1);   // Posició de destí

        // Calculem la direcció del moviment (amunt o avall, esquerra o dreta)
        int dr;
        if (p2.getFila() - p1.getFila() > 0)
            dr = 1; // Moviment cap avall
        else
            dr = -1; // Moviment cap amunt

        int dc;
        if (p2.getColumna() - p1.getColumna() > 0)
            dc = 1; // Moviment cap a la dreta
        else
            dc = -1; // Moviment cap a l'esquerra

        // Comencem just després de la posició inicial
        int f = p1.getFila() + dr;
        int c = p1.getColumna() + dc;
        bool fitxaCapturada = false;

        // Avancem pas a pas fins arribar a la posició final
        while (f != p2.getFila() && c != p2.getColumna() && !fitxaCapturada) {
            // Si trobem una fitxa, la suprimim perquè ha estat capturada
            if (!m_tauler[f][c]->esBuida()) {
                delete m_tauler[f][c];                // Alliberem la memòria de la fitxa
                m_tauler[f][c] = new Fitxa();         // Substituïm per una fitxa buida
                fitxaCapturada = true;                // Marquem que ja hem eliminat una fitxa en aquest tram
            }
            else {
                f += dr;                              // Seguim avançant en la direcció
                c += dc;
            }
        }
    }
}

void Tauler::bufarSiNoHaFetMillorCaptura(const Posicio& origen, const Moviment& movimentFet, ColorFitxa tornActual) {
    // Comptem les captures fetes (número de salts menys 1)
    int capturesFetes = movimentFet.getNPosicions() - 1;
    bool sHaDeBufar = false;

    // Només s'ha de bufar si no s'ha fet cap captura
    if (capturesFetes == 0) {
        for (int fila = 0; fila < N_FILES; ++fila) {
            for (int col = 0; col < N_COLUMNES; ++col) {
                // Ignorem la fitxa que s'acaba de moure
                bool esMateixaPosicio = false;
                if (fila == origen.getFila() && col == origen.getColumna())
                    esMateixaPosicio = true;

                if (!esMateixaPosicio) {
                    Fitxa* fitxa = m_tauler[fila][col];

                    // Comprovem si hi ha una fitxa del jugador actual
                    if (!fitxa->esBuida() && fitxa->getColor() == tornActual) {
                        // Mirem si té algun moviment que impliqui captura de més d'una fitxa
                        for (int i = 0; i < fitxa->getNumMoviments(); ++i) {
                            int nPosicions = fitxa->getMoviment(i).getNPosicions();
                            if (nPosicions > 2) {
                                sHaDeBufar = true;

                                // Simulem sortida del bucle intern
                                i = fitxa->getNumMoviments();
                            }
                        }
                    }
                }

                // Si ja sabem que s'ha de bufar, sortim del bucle de columnes
                if (sHaDeBufar) {
                    col = N_COLUMNES;
                }
            }

            // I també del bucle de files
            if (sHaDeBufar) {
                fila = N_FILES;
            }
        }
    }

    // Si cal bufar, eliminem la fitxa que s’ha mogut
    if (sHaDeBufar) {
        int f = origen.getFila();
        int c = origen.getColumna();
        delete m_tauler[f][c];
        m_tauler[f][c] = new Fitxa(); // Posar una fitxa buida
    }
}

const Fitxa& Tauler::getFitxa(int fila, int columna) const {
    return *m_tauler[fila][columna];  // accés per punter
}

void Tauler::visualitza() const {
    // Dibuixa el tauler de fons
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    // Recorre totes les files i columnes del tauler
    for (int fila = 0; fila < N_FILES; ++fila) {
        for (int col = 0; col < N_COLUMNES; ++col) {
            // Comprova que hi ha una fitxa vàlida a la casella
            if (m_tauler[fila][col] != nullptr) {
                bool esBuida = m_tauler[fila][col]->esBuida();
                if (!esBuida) {
                    // Calcula la posició en píxels on dibuixar la fitxa
                    int x = POS_X_TAULER + CASELLA_INICIAL_X + col * AMPLADA_CASELLA;
                    int y = POS_Y_TAULER + CASELLA_INICIAL_Y + fila * ALCADA_CASELLA;

                    // Dibuixa la fitxa en pantalla
                    m_tauler[fila][col]->visualitza(x, y);
                }
            }
        }
    }
}


ColorFitxa Tauler::getColorFitxa(const Posicio& pos) const {
    // Inicialment retornem un valor per defecte que indica absència de fitxa
    ColorFitxa color = CAP_GUANYADOR;

    // Obtenim la fila i la columna de la posició
    int fila = pos.getFila();
    int col = pos.getColumna();

    // Comprovem que la posició estigui dins dels límits del tauler
    bool dinsFiles = fila >= 0 && fila < N_FILES;
    bool dinsColumnes = col >= 0 && col < N_COLUMNES;

    if (dinsFiles && dinsColumnes) {
        // Retornem el color real de la fitxa que hi ha a la casella
        color = m_tauler[fila][col]->getColor();
    }

    return color;
}


void Tauler::executaMoviment(const Moviment& moviment)
{
    if (moviment.getNPosicions() >= 2) {
        // 1. Copia la fitxa de la posició inicial
        Posicio origen = moviment.getPosicio(0);
        Fitxa fitxa = *m_tauler[origen.getFila()][origen.getColumna()];

        // 2. Buida la casella d’origen
        *m_tauler[origen.getFila()][origen.getColumna()] = Fitxa();

        // 3. Elimina les fitxes capturades
        eliminaFitxesCapturades(moviment);

        // 4. Mou pas a pas fins al final
        Posicio desti = moviment.getDesti();
        *m_tauler[desti.getFila()][desti.getColumna()] = fitxa;

        // 5. Comprova si s'ha de convertir en dama
        bool esBlancaADama = (fitxa.getColor() == COLOR_BLANC && desti.getFila() == 0);
        bool esNegraADama = (fitxa.getColor() == COLOR_NEGRE && desti.getFila() == N_FILES - 1);
        if (esBlancaADama || esNegraADama) {
            fitxa.convertirEnDama();
            *m_tauler[desti.getFila()][desti.getColumna()] = fitxa;
        }
    }
}

bool Tauler::teMoviments(ColorFitxa color) const {
    // Suposem inicialment que no hi ha cap moviment disponible
    bool hiHaMoviment = false;

    // Recorrem totes les caselles del tauler
    for (int fila = 0; fila < N_FILES && !hiHaMoviment; ++fila) {
        for (int col = 0; col < N_COLUMNES && !hiHaMoviment; ++col) {

            // Comprovem que hi ha una fitxa a la casella
            bool casellaValida = (m_tauler[fila][col] != nullptr);
            bool noEsBuida = false;
            if (casellaValida)
                noEsBuida = !m_tauler[fila][col]->esBuida();

            // Si hi ha una fitxa no buida i del color que estem comprovant
            if (casellaValida && noEsBuida && m_tauler[fila][col]->getColor() == color) {
                vector<Posicio> moviments;
                Posicio pos(fila, col);

                // Obtenim els moviments possibles per a aquesta fitxa
                getPosicionsPossibles(pos, moviments);

                // Si trobem almenys un moviment, ho marquem i sortirem dels bucles
                if (!moviments.empty()) {
                    hiHaMoviment = true;
                }
            }
        }
    }

    return hiHaMoviment;
}

bool Tauler::esMovimentCapturaReal(const Moviment& m) const {
    // Inicialment suposem que no hi ha cap captura
    bool capturaTrobada = false;

    // Recorrem tots els trams entre posicions consecutives del moviment
    for (int i = 0; i < m.getNPosicions() - 1 && !capturaTrobada; ++i) {
        Posicio p1 = m.getPosicio(i);         // Posició d'origen
        Posicio p2 = m.getPosicio(i + 1);     // Posició de destí

        // Determinem la direcció vertical (dr) i horitzontal (dc)
        int dr;
        if (p2.getFila() - p1.getFila() > 0)
            dr = 1;
        else
            dr = -1;

        int dc;
        if (p2.getColumna() - p1.getColumna() > 0)
            dc = 1;
        else
            dc = -1;

        int f = p1.getFila() + dr;
        int c = p1.getColumna() + dc;

        // Recorrem les caselles entre p1 i p2
        while (f != p2.getFila() && c != p2.getColumna() && !capturaTrobada) {
            if (!m_tauler[f][c]->esBuida()) {
                // Comprovem si la fitxa trobada és d'un color diferent al de l'origen
                ColorFitxa colorOrigen = m_tauler[p1.getFila()][p1.getColumna()]->getColor();
                ColorFitxa colorActual = m_tauler[f][c]->getColor();

                if (colorActual != colorOrigen) {
                    capturaTrobada = true; // Confirmem que hi ha una captura real
                }
            }

            // Avancem cap a la següent casella
            f += dr;
            c += dc;
        }
    }

    return capturaTrobada;
}
