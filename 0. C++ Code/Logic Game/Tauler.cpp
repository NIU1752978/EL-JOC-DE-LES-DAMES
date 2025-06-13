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
    inicialitzaTaulerBuit();
}

Tauler::~Tauler() {
    for (int i = 0; i < N_FILES; ++i) {
        for (int j = 0; j < N_COLUMNES; ++j) {
            delete m_tauler[i][j];
            m_tauler[i][j] = nullptr;
        }
    }
}

void Tauler::inicialitzaTaulerBuit() {
    for (int i = 0; i < N_FILES; ++i) {
        for (int j = 0; j < N_COLUMNES; ++j) {
            delete m_tauler[i][j]; // esborra si hi havia alguna fitxa
            m_tauler[i][j] = new Fitxa();  // fitxa buida
        }
    }
}

void Tauler::inicialitza(const std::string& nomFitxer)
{
    inicialitzaTaulerBuit();

    std::ifstream fitxer(nomFitxer);
    if (fitxer.is_open())
    {
        char tipusChar;
        std::string posicioStr;

        while (fitxer >> tipusChar >> posicioStr)
        {
            TipusFitxa tipusFitxa;
            ColorFitxa colorFitxa;
            bool valid = true;

            switch (tipusChar)
            {
            case 'O':
                tipusFitxa = TIPUS_NORMAL;
                colorFitxa = COLOR_BLANC;
                break;
            case 'X':
                tipusFitxa = TIPUS_NORMAL;
                colorFitxa = COLOR_NEGRE;
                break;
            case 'D':
                tipusFitxa = TIPUS_DAMA;
                colorFitxa = COLOR_BLANC;
                break;
            case 'R':
                tipusFitxa = TIPUS_DAMA;
                colorFitxa = COLOR_NEGRE;
                break;
            default:
                valid = false;
                break;
            }

            if (valid)
            {
                Posicio pos(posicioStr);
                int fila = pos.getFila();
                int col = pos.getColumna();

                if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES)
                {
                    Fitxa* fitxa = new Fitxa(tipusFitxa, colorFitxa);
                    delete m_tauler[fila][col];
                    m_tauler[fila][col] = fitxa;

                }
            }
        }

        fitxer.close();
    }
}

void Tauler::establirFitxa(const Posicio& pos, Fitxa* fitxa)
{
    int fila = pos.getFila();
    int col = pos.getColumna();
    delete m_tauler[fila][col];
    m_tauler[fila][col] = fitxa;
}

void Tauler::actualitzaMovimentsValids()
{
    for (int fila = 0; fila < N_FILES; ++fila)
    {
        for (int columna = 0; columna < N_COLUMNES; ++columna)
        {
            Fitxa& fitxa = *m_tauler[fila][columna];

            if (!fitxa.esBuida())
            {
                fitxa.netejaMoviments();

                if (fitxa.getTipus() == TIPUS_NORMAL)
                {
                    afegeixMovimentsFitxaNormal(fila, columna);
                }
                else if (fitxa.getTipus() == TIPUS_DAMA)
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

    if (esPosicioDinsTauler(filaOri, colOri) && esPosicioDinsTauler(filaDes, colDes)) {
        Fitxa* fitxa = m_tauler[filaOri][colOri];

        if (!fitxa->esBuida() && fitxa->getColor() == tornActual) {
            for (int i = 0; i < fitxa->getNumMoviments(); ++i) {
                Moviment m = fitxa->getMoviment(i);
                if (m.getDesti() == desti) {
                    movimentSeleccionat = m;
                    movimentValid = true;
                }
            }

            if (movimentValid) {
                int maxPosicions = 0;
                for (int i = 0; i < fitxa->getNumMoviments(); ++i) {
                    Moviment m = fitxa->getMoviment(i);
                    if (m.getNPosicions() >= 2) {
                        const Posicio& p1 = m.getPosicio(0);
                        const Posicio& p2 = m.getPosicio(1);
                        if (abs(p1.getFila() - p2.getFila()) > 1 && abs(p1.getColumna() - p2.getColumna()) > 1) {
                            maxPosicions = std::max(maxPosicions, m.getNPosicions());
                        }
                    }
                }

                int posicionsFetes = movimentSeleccionat.getNPosicions();
                bool esCaptura = esMovimentCapturaReal(movimentSeleccionat);

                bool movimentAcceptat = true;

                if (esCaptura && posicionsFetes < maxPosicions) {
                    eliminaFitxesCapturades(movimentSeleccionat);
                    delete m_tauler[filaOri][colOri];
                    m_tauler[filaOri][colOri] = new Fitxa();
                    movimentAcceptat = false;
                }

                if (!esCaptura) {
                    bool algunaCapturaAlTauler = false;

                    for (int fila = 0; fila < N_FILES && !algunaCapturaAlTauler; ++fila) {
                        for (int col = 0; col < N_COLUMNES && !algunaCapturaAlTauler; ++col) {
                            Fitxa* fitxaTemp = m_tauler[fila][col];
                            if (!fitxaTemp->esBuida() && fitxaTemp->getColor() == tornActual) {
                                for (int i = 0; i < fitxaTemp->getNumMoviments(); ++i) {
                                    if (esMovimentCapturaReal(fitxaTemp->getMoviment(i))) {
                                        algunaCapturaAlTauler = true;
                                    }
                                }
                            }
                        }
                    }

                    if (algunaCapturaAlTauler) {
                        delete m_tauler[filaOri][colOri];
                        m_tauler[filaOri][colOri] = new Fitxa();
                        movimentAcceptat = false;
                    }
                }

                if (movimentAcceptat) {
                    eliminaFitxesCapturades(movimentSeleccionat);
                    delete m_tauler[filaDes][colDes];
                    m_tauler[filaDes][colDes] = fitxa;
                    m_tauler[filaOri][colOri] = new Fitxa();

                    if ((fitxa->getColor() == COLOR_BLANC && filaDes == 0) ||
                        (fitxa->getColor() == COLOR_NEGRE && filaDes == N_FILES - 1)) {
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

    for (int i = 0; i < fitxa.getNumMoviments(); ++i) {
        Moviment m = fitxa.getMoviment(i);
        if (m.getNPosicions() > 0) {
            Posicio desti = m.getDesti();

            bool mateixaPosicio = (desti.getFila() == origen.getFila()) && (desti.getColumna() == origen.getColumna());

            if (!mateixaPosicio) {
                bool jaExisteix = false;
                size_t j = 0;
                while (j < destins.size()) {
                    bool mateixaFila = (destins[j].getFila() == desti.getFila());
                    bool mateixaColumna = (destins[j].getColumna() == desti.getColumna());
                    if (mateixaFila && mateixaColumna) {
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
    for (size_t i = 0; i < indexos.size(); ++i) indexos[i] = static_cast<int>(i);

    // Ordenem sense lambda amb returns
    for (size_t i = 0; i < indexos.size(); ++i) {
        for (size_t j = i + 1; j < indexos.size(); ++j) {
            bool millor = false;
            if (captures[indexos[j]] > captures[indexos[i]]) {
                millor = true;
            }
            else if (captures[indexos[j]] == captures[indexos[i]]) {
                if (dames[indexos[j]] > dames[indexos[i]]) {
                    millor = true;
                }
            }

            if (millor) {
                int temp = indexos[i];
                indexos[i] = indexos[j];
                indexos[j] = temp;
            }
        }
    }

    // Afegim les posicions finals ordenades
    for (size_t i = 0; i < indexos.size(); ++i) {
        posicionsPossibles.push_back(destins[indexos[i]]);
    }
}

std::string Tauler::toString() const {
    std::ostringstream resultat;

    for (int fila = 0; fila < N_FILES; fila++) {
        char numeroFila = '0' + (N_FILES - fila);
        resultat << numeroFila << ": ";

        for (int columna = 0; columna < N_COLUMNES; columna++) {
            char c = '_';
            const Fitxa& fitxa = *m_tauler[fila][columna];

            if (!fitxa.esBuida()) {
                if (fitxa.getTipus() == TIPUS_DAMA)
                    c = (fitxa.getColor() == COLOR_BLANC) ? 'D' : 'R';
                else
                    c = (fitxa.getColor() == COLOR_BLANC) ? 'O' : 'X';
            }

            resultat << c << ' ';
        }

        resultat << '\n';
    }

    resultat << "   a b c d e f g h\n";
    return resultat.str();
}

void Tauler::afegeixMovimentsFitxaNormal(int fila, int columna) {
    Fitxa& fitxa = *m_tauler[fila][columna];

    bool esNormal = (fitxa.getTipus() == TIPUS_NORMAL);
    bool noEsBuida = !fitxa.esBuida();

    if (esNormal && noEsBuida) {

        Moviment possiblesCaptures[MAX_MOVIMENTS];
        int nCaptures = 0;
        bool capturaTrobada = false;

        Moviment movimentInicial;
        movimentInicial.afegirPosicio(Posicio(fila, columna));

        obtenirMovimentsDesDePosicio(fitxa, fila, columna, movimentInicial, possiblesCaptures, nCaptures, capturaTrobada);

        // Afegim captures si n'hi ha
        for (int i = 0; i < nCaptures; i++) {
            fitxa.afegeixMovimentValid(possiblesCaptures[i]);
        }

        // I també afegim moviments normals
        int dir = (fitxa.getColor() == COLOR_BLANC) ? -1 : 1;
        for (int dc = -1; dc <= 1; dc += 2) {
            int filaNova = fila + dir;
            int columnaNova = columna + dc;

            bool dinsTauler = esPosicioDinsTauler(filaNova, columnaNova);
            bool destiBuida = dinsTauler && m_tauler[filaNova][columnaNova]->esBuida();

            if (dinsTauler && destiBuida) {
                Moviment m;
                m.afegirPosicio(Posicio(fila, columna));
                m.afegirPosicio(Posicio(filaNova, columnaNova));
                fitxa.afegeixMovimentValid(m);
            }
        }
    }
}

void Tauler::afegeixMovimentsFitxaDama(int fila, int columna) {
    Fitxa& fitxa = *m_tauler[fila][columna];
    fitxa.netejaMoviments();

    Moviment movimentInicial;
    movimentInicial.afegirPosicio(Posicio(fila, columna));

    Moviment possiblesMoviments[MAX_MOVIMENTS];
    int nMoviments = 0;

    obtenirMovimentsDamaDesDePosicio(fitxa, fila, columna, movimentInicial,
        possiblesMoviments, nMoviments);

    for (int i = 0; i < nMoviments; ++i) {
        fitxa.afegeixMovimentValid(possiblesMoviments[i]);
    }
}

bool Tauler::esCapturaDisponible(const Fitxa& fitxa, int fila, int columna) const {
    bool captura = false;

    if (!fitxa.esBuida()) {
        ColorFitxa colorPropi = fitxa.getColor();
        TipusFitxa tipus = fitxa.getTipus();

        const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

        for (int i = 0; i < 4 && !captura; ++i) {
            int dr = dirs[i][0];
            int dc = dirs[i][1];

            if (tipus == TIPUS_NORMAL) {
                int direccio = (colorPropi == COLOR_BLANC) ? -1 : 1;

                if (dr == direccio) {
                    int fMig = fila + dr;
                    int cMig = columna + dc;
                    int fDesti = fila + 2 * dr;
                    int cDesti = columna + 2 * dc;

                    if (esPosicioDinsTauler(fMig, cMig) && esPosicioDinsTauler(fDesti, cDesti)) {
                        const Fitxa& mig = *m_tauler[fMig][cMig];
                        const Fitxa& desti = *m_tauler[fDesti][cDesti];

                        if (!mig.esBuida() && mig.getColor() != colorPropi && desti.esBuida()) {
                            captura = true;
                        }
                    }
                }

            }
            else if (tipus == TIPUS_DAMA) {
                int f = fila + dr;
                int c = columna + dc;
                bool trobatContrari = false;

                while (esPosicioDinsTauler(f, c) && !captura) {
                    const Fitxa& actual = *m_tauler[f][c];

                    if (actual.esBuida()) {
                        if (trobatContrari) {
                            captura = true;
                        }
                        f += dr;
                        c += dc;
                    }
                    else {
                        if (actual.getColor() == colorPropi || trobatContrari) {
                            f = N_FILES; // sortir del while
                            c = N_COLUMNES;
                        }
                        else {
                            trobatContrari = true;
                            f += dr;
                            c += dc;
                        }
                    }
                }
            }
        }
    }

    return captura;
}

bool Tauler::esPosicioDinsTauler(int fila, int columna) const {
    return (fila >= 0 && fila < N_FILES && columna >= 0 && columna < N_COLUMNES);
}

bool Tauler::hiHaFitxaContraria(int fila, int col, ColorFitxa colorPropi) const
{
    bool resultat = false;

    if (esPosicioDinsTauler(fila, col)) {
        const Fitxa& f = *m_tauler[fila][col];

        if (!f.esBuida()) {
            resultat = (f.getColor() != colorPropi);
        }
    }

    return resultat;
}

bool Tauler::esCapturaPossible(const Fitxa& fitxa, int fila, int columna) const {
    bool capturaPossible = false;

    if (!fitxa.esBuida() && esPosicioDinsTauler(fila, columna)) {
        int direccio = (fitxa.getColor() == COLOR_BLANC) ? -1 : 1;

        // Provar totes dues diagonals
        for (int dc = -1; dc <= 1; dc += 2) {
            int filaMig = fila + direccio;
            int colMig = columna + dc;

            int filaDest = fila + 2 * direccio;
            int colDest = columna + 2 * dc;

            bool dinsTauler = esPosicioDinsTauler(filaDest, colDest) && esPosicioDinsTauler(filaMig, colMig);

            if (dinsTauler) {
                const Fitxa& mig = *m_tauler[filaMig][colMig];
                const Fitxa& desti = *m_tauler[filaDest][colDest];

                bool hiHaContraria = !mig.esBuida() && mig.getColor() != fitxa.getColor();
                bool destiBuida = desti.esBuida();

                if (hiHaContraria && destiBuida) {
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

void Tauler::obtenirMovimentsDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual, Moviment possiblesMoviments[], int& nMoviments, bool& capturaTrobada)
{
    const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    bool haTrobatCaptura = false;
    int direccio = (fitxa.getColor() == COLOR_BLANC) ? -1 : 1;

    for (int i = 0; i < 4; ++i) {
        int dr = dirs[i][0], dc = dirs[i][1];

        // ❗ Només permet captures cap endavant per fitxes normals
        if (dr == direccio) {
            int filaMig = fila + dr;
            int colMig = columna + dc;
            int filaDest = fila + 2 * dr;
            int colDest = columna + 2 * dc;

            if (esPosicioDinsTauler(filaMig, colMig) && esPosicioDinsTauler(filaDest, colDest)) {
                if (hiHaFitxaContraria(filaMig, colMig, fitxa.getColor()) && m_tauler[filaDest][colDest]->esBuida()) {

                    haTrobatCaptura = true;
                    capturaTrobada = true;

                    Fitxa fitxaCapturada, fitxaOriginal;
                    Posicio origen(fila, columna);
                    Posicio mig(filaMig, colMig);
                    Posicio desti(filaDest, colDest);

                    simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                    Moviment nouMoviment = movimentActual;
                    nouMoviment.afegirPosicio(desti);

                    Fitxa& novaFitxa = *m_tauler[filaDest][colDest];
                    obtenirMovimentsDesDePosicio(novaFitxa, filaDest, colDest, nouMoviment, possiblesMoviments, nMoviments, capturaTrobada);

                    if (nouMoviment.getNPosicions() > 1 && nMoviments < MAX_MOVIMENTS)
                        possiblesMoviments[nMoviments++] = nouMoviment;

                    desfesCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);
                }
            }
        }
    }

    if (!haTrobatCaptura) {
        if (movimentActual.getNPosicions() > 1) {
            if (nMoviments < MAX_MOVIMENTS)
                possiblesMoviments[nMoviments++] = movimentActual;
        }
        else if (!capturaTrobada) {
            int direccio = (fitxa.getColor() == COLOR_BLANC) ? -1 : 1;
            for (int dc = -1; dc <= 1; dc += 2) {
                int nf = fila + direccio;
                int nc = columna + dc;
                if (esPosicioDinsTauler(nf, nc) && m_tauler[nf][nc]->esBuida()) {
                    Moviment m;
                    m.afegirPosicio(Posicio(fila, columna));
                    m.afegirPosicio(Posicio(nf, nc));

                    if (nMoviments < MAX_MOVIMENTS)
                        possiblesMoviments[nMoviments++] = m;
                }
            }
        }
    }
}

void Tauler::obtenirMovimentsDamaDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual, Moviment possiblesMoviments[], int& nMoviments)
{
    const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    bool capturaTrobada = false;

    for (int d = 0; d < 4; ++d) {
        int dr = dirs[d][0];
        int dc = dirs[d][1];
        int f = fila + dr;
        int c = columna + dc;

        bool trobatContrari = false;
        int filaContrari = -1;
        int columnaContrari = -1;
        bool continuar = true;

        while (esPosicioDinsTauler(f, c) && continuar) {
            if (m_tauler[f][c]->esBuida()) {
                if (trobatContrari) {
                    // Captura vàlida
                    Posicio origen(fila, columna);
                    Posicio mig(filaContrari, columnaContrari);
                    Posicio desti(f, c);

                    Fitxa fitxaCapturada, fitxaOriginal;
                    simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                    Moviment nouMoviment = movimentActual;
                    nouMoviment.afegirPosicio(desti);

                    Fitxa novaFitxa = *m_tauler[f][c];
                    obtenirMovimentsDamaDesDePosicio(novaFitxa, f, c, nouMoviment,
                        possiblesMoviments, nMoviments);

                    if (nouMoviment.getNPosicions() > 1 && nMoviments < MAX_MOVIMENTS)
                        possiblesMoviments[nMoviments++] = nouMoviment;

                    desfesCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);
                    capturaTrobada = true;
                }

                f += dr;
                c += dc;
            }
            else {
                bool esFitxaContraria = (m_tauler[f][c]->getColor() != fitxa.getColor());
                bool esPrimeraFitxa = !trobatContrari;

                if (esPrimeraFitxa && esFitxaContraria) {
                    trobatContrari = true;
                    filaContrari = f;
                    columnaContrari = c;
                    f += dr;
                    c += dc;
                }
                else {
                    continuar = false;
                }
            }
        }
    }

    // Afegeix moviments normals si només tenim la posició inicial
    if (movimentActual.getNPosicions() == 1) {
        for (int d = 0; d < 4; ++d) {
            int dr = dirs[d][0];
            int dc = dirs[d][1];
            int f = fila + dr;
            int c = columna + dc;

            bool continuar = true;
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
    for (int i = 0; i < m.getNPosicions() - 1; ++i) {
        Posicio p1 = m.getPosicio(i);
        Posicio p2 = m.getPosicio(i + 1);

        int dr = (p2.getFila() - p1.getFila()) > 0 ? 1 : -1;
        int dc = (p2.getColumna() - p1.getColumna()) > 0 ? 1 : -1;

        int f = p1.getFila() + dr;
        int c = p1.getColumna() + dc;

        bool fitxaCapturada = false;

        while (f != p2.getFila() && c != p2.getColumna() && !fitxaCapturada) {
            if (!m_tauler[f][c]->esBuida()) {
                delete m_tauler[f][c];
                m_tauler[f][c] = new Fitxa(); // eliminar fitxa capturada
                fitxaCapturada = true;
            }
            else {
                f += dr;
                c += dc;
            }
        }
    }
}

void Tauler::bufarSiNoHaFetMillorCaptura(const Posicio& origen, const Moviment& movimentFet, ColorFitxa tornActual)
{
    int capturesFetes = movimentFet.getNPosicions() - 1;
    bool sHaDeBufar = false;

    if (capturesFetes == 0) {
        for (int fila = 0; fila < N_FILES; ++fila) {
            for (int col = 0; col < N_COLUMNES; ++col) {
                bool esMateixa = (fila == origen.getFila()) && (col == origen.getColumna());

                if (!esMateixa) {
                    Fitxa* fitxa = m_tauler[fila][col];
                    if (!fitxa->esBuida() && fitxa->getColor() == tornActual) {
                        for (int i = 0; i < fitxa->getNumMoviments(); ++i) {
                            if (fitxa->getMoviment(i).getNPosicions() > 2) {
                                sHaDeBufar = true;
                                i = fitxa->getNumMoviments(); // sortir del bucle sense `break`
                            }
                        }
                    }
                }

                if (sHaDeBufar) col = N_COLUMNES; // sortir del bucle exterior sense `break`
            }

            if (sHaDeBufar) fila = N_FILES;
        }
    }

    if (sHaDeBufar) {
        delete m_tauler[origen.getFila()][origen.getColumna()];
        m_tauler[origen.getFila()][origen.getColumna()] = new Fitxa();
    }
}

const Fitxa& Tauler::getFitxa(int fila, int columna) const {
    return *m_tauler[fila][columna];  // accés per punter
}

void Tauler::visualitza() const {

    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    for (int fila = 0; fila < N_FILES; ++fila) {
        for (int col = 0; col < N_COLUMNES; ++col) {
            if (m_tauler[fila][col] != nullptr && !m_tauler[fila][col]->esBuida()) {
                int x = POS_X_TAULER + CASELLA_INICIAL_X + col * AMPLADA_CASELLA;
                int y = POS_Y_TAULER + CASELLA_INICIAL_Y + fila * ALCADA_CASELLA;
                m_tauler[fila][col]->visualitza(x, y);
            }
        }
    }
}

ColorFitxa Tauler::getColorFitxa(const Posicio& pos) const {
    ColorFitxa color = CAP_GUANYADOR;

    int fila = pos.getFila();
    int col = pos.getColumna();

    if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) {
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

bool Tauler::teMoviments(ColorFitxa color) const
{
    bool hiHaMoviment = false;

    for (int fila = 0; fila < N_FILES && !hiHaMoviment; ++fila)
    {
        for (int col = 0; col < N_COLUMNES && !hiHaMoviment; ++col)
        {
            if (m_tauler[fila][col] != nullptr &&
                !m_tauler[fila][col]->esBuida() &&
                m_tauler[fila][col]->getColor() == color)
            {
                vector<Posicio> moviments;
                Posicio p(fila, col);
                getPosicionsPossibles(p, moviments);

                if (!moviments.empty())
                    hiHaMoviment = true;
            }
        }
    }

    return hiHaMoviment;
}

bool Tauler::esMovimentCapturaReal(const Moviment& m) const {
    bool capturaTrobada = false;

    for (int i = 0; i < m.getNPosicions() - 1 && !capturaTrobada; ++i) {
        Posicio p1 = m.getPosicio(i);
        Posicio p2 = m.getPosicio(i + 1);

        int dr = (p2.getFila() - p1.getFila()) > 0 ? 1 : -1;
        int dc = (p2.getColumna() - p1.getColumna()) > 0 ? 1 : -1;

        int f = p1.getFila() + dr;
        int c = p1.getColumna() + dc;

        while (f != p2.getFila() && c != p2.getColumna() && !capturaTrobada) {
            if (!m_tauler[f][c]->esBuida()) {
                ColorFitxa colorOrigen = m_tauler[p1.getFila()][p1.getColumna()]->getColor();
                ColorFitxa colorActual = m_tauler[f][c]->getColor();

                if (colorActual != colorOrigen) {
                    capturaTrobada = true;
                }
            }
            f += dr;
            c += dc;
        }
    }

    return capturaTrobada;
}