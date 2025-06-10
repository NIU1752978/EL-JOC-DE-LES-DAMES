#include "Tauler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Fitxa.h"
using namespace std;

Tauler::Tauler()
{
    inicialitzaTaulerBuit();
    tornActual = COLOR_BLANC;
}

void Tauler::inicialitzaTaulerBuit() {
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int columna = 0; columna < N_COLUMNES; columna++) {
            m_tauler[fila][columna] = Fitxa(); // TIPUS_EMPTY per defecte
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
                    Fitxa fitxa(tipusFitxa, colorFitxa);
                    establirFitxa(pos, fitxa);
                }
            }
        }

        fitxer.close();
    }
}

void Tauler::establirFitxa(const Posicio& posicio, const Fitxa& fitxa)
{
    int fila = posicio.getFila();
    int columna = posicio.getColumna();

    if (fila >= 0 && fila < N_FILES && columna >= 0 && columna < N_COLUMNES) {
        m_tauler[fila][columna] = fitxa;
    }
}

void Tauler::actualitzaMovimentsValids()
{
    for (int fila = 0; fila < N_FILES; ++fila)
    {
        for (int columna = 0; columna < N_COLUMNES; ++columna)
        {
            Fitxa& fitxa = m_tauler[fila][columna];

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

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti)
{
    int filaOri = origen.getFila();
    int colOri = origen.getColumna();

    int filaDes = desti.getFila();
    int colDes = desti.getColumna();

    bool resultatFinal = true;

    if (!esPosicioDinsTauler(filaOri, colOri) || !esPosicioDinsTauler(filaDes, colDes)) {
        resultatFinal = false;
    }
    else {
        Fitxa& fitxa = m_tauler[filaOri][colOri];

        if (fitxa.esBuida() || fitxa.getColor() != tornActual) {
            resultatFinal = false;
        }
        else {
            bool movimentTrobat = false;
            Moviment movimentSeleccionat;

            for (int i = 0; i < fitxa.getNumMoviments(); ++i) {
                Moviment m = fitxa.getMoviment(i);
                if (m.getDesti() == desti) {
                    movimentSeleccionat = m;
                    movimentTrobat = true;
                }
            }

            if (!movimentTrobat) {
                resultatFinal = false;
            }
            else {
                eliminaFitxesCapturades(movimentSeleccionat);
                m_tauler[filaDes][colDes] = fitxa;
                m_tauler[filaOri][colOri] = Fitxa();

                if ((fitxa.getColor() == COLOR_BLANC && filaDes == 0) ||
                    (fitxa.getColor() == COLOR_NEGRE && filaDes == N_FILES - 1)) {
                    m_tauler[filaDes][colDes].convertirEnDama();
                }

                bufarSiNoHaFetMillorCaptura(origen, movimentSeleccionat);
            }
        }
    }

    return resultatFinal;
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]) {
    nPosicions = 0;

    int fila = origen.getFila();
    int columna = origen.getColumna();
    Fitxa& fitxa = m_tauler[fila][columna];

    // Arrays auxiliars
    Posicio destins[MAX_MOVIMENTS];
    int captures[MAX_MOVIMENTS];
    int dames[MAX_MOVIMENTS];

    int total = 0;

    // Recollir moviments vàlids
    for (int i = 0; i < fitxa.getNumMoviments(); ++i) {
        Moviment m = fitxa.getMoviment(i);
        if (m.getNPosicions() > 0) {
            Posicio desti = m.getDesti();

            // Només afegim si no és la mateixa posició d'origen
            if (!(desti == origen)) {
                // Comprovem si ja existeix
                bool repetit = false;
                for (int j = 0; j < total; ++j) {
                    if (destins[j] == desti) {
                        repetit = true;
                    }
                }

                if (!repetit && total < MAX_MOVIMENTS) {
                    destins[total] = desti;
                    captures[total] = m.getNPosicions() - 1;
                    dames[total] = m.getNumDamesCapturades(*this);
                    total++;
                }
            }
        }
    }

    // Ordenació manual (bubble sort)
    for (int i = 0; i < total - 1; ++i) {
        for (int j = i + 1; j < total; ++j) {
            bool millor = false;

            if (captures[i] < captures[j]) {
                millor = true;
            }
            else if (captures[i] == captures[j] && dames[i] < dames[j]) {
                millor = true;
            }

            if (millor) {
                // Intercanvi manual
                Posicio tmpPos = destins[i];
                destins[i] = destins[j];
                destins[j] = tmpPos;

                int tmpCap = captures[i];
                captures[i] = captures[j];
                captures[j] = tmpCap;

                int tmpDam = dames[i];
                dames[i] = dames[j];
                dames[j] = tmpDam;
            }
        }
    }

    // Copiar resultat
    for (int i = 0; i < total; ++i) {
        posicionsPossibles[nPosicions] = destins[i];
        nPosicions++;
    }
}

std::string Tauler::toString() const {
    std::ostringstream resultat;

    for (int fila = 0; fila < N_FILES; fila++) {
        char numeroFila = '0' + (N_FILES - fila);
        resultat << numeroFila << ": ";

        for (int columna = 0; columna < N_COLUMNES; columna++) {
            char c = '_';
            const Fitxa& fitxa = m_tauler[fila][columna];

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
    Fitxa& fitxa = m_tauler[fila][columna];

    bool esNormal = (fitxa.getTipus() == TIPUS_NORMAL);
    bool noEsBuida = !fitxa.esBuida();

    if (esNormal && noEsBuida) {
        fitxa.netejaMoviments();

        Moviment possiblesCaptures[MAX_MOVIMENTS];
        int nCaptures = 0;
        bool capturaTrobada = false;

        Moviment movimentInicial;
        movimentInicial.afegeixPosicio(Posicio(fila, columna));

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
            bool destiBuida = dinsTauler && m_tauler[filaNova][columnaNova].esBuida();

            if (dinsTauler && destiBuida) {
                Moviment m;
                m.afegeixPosicio(Posicio(fila, columna));
                m.afegeixPosicio(Posicio(filaNova, columnaNova));
                fitxa.afegeixMovimentValid(m);
            }
        }
    }
}

void Tauler::afegeixMovimentsFitxaDama(int fila, int columna) {
    Fitxa& fitxa = m_tauler[fila][columna];
    fitxa.netejaMoviments();

    Moviment movimentInicial;
    movimentInicial.afegeixPosicio(Posicio(fila, columna));

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
                        const Fitxa& mig = m_tauler[fMig][cMig];
                        const Fitxa& desti = m_tauler[fDesti][cDesti];

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
                    const Fitxa& actual = m_tauler[f][c];

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
        const Fitxa& f = m_tauler[fila][col];

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
                const Fitxa& mig = m_tauler[filaMig][colMig];
                const Fitxa& desti = m_tauler[filaDest][colDest];

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
    fitxaCapturada = m_tauler[mig.getFila()][mig.getColumna()];
    fitxaOriginal = m_tauler[origen.getFila()][origen.getColumna()];

    // Buida la casella origen i la capturada
    m_tauler[origen.getFila()][origen.getColumna()] = Fitxa(); // Buit
    m_tauler[mig.getFila()][mig.getColumna()] = Fitxa();       // Buit

    // Mou la fitxa original a destí
    m_tauler[desti.getFila()][desti.getColumna()] = fitxaOriginal;
}

void Tauler::desfesCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, const Fitxa& fitxaCapturada, const Fitxa& fitxaOriginal) {
    // Tornem la peça original a la seva posició
    m_tauler[origen.getFila()][origen.getColumna()] = fitxaOriginal;

    // Tornem la peça capturada al seu lloc
    m_tauler[mig.getFila()][mig.getColumna()] = fitxaCapturada;

    // Eliminem la peça que havia anat a destí
    m_tauler[desti.getFila()][desti.getColumna()] = Fitxa();
}

void Tauler::obtenirMovimentsDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual, Moviment possiblesMoviments[], int& nMoviments, bool& capturaTrobada)
{

    const int dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    bool haTrobatCaptura = false;

    for (int i = 0; i < 4; ++i) {
        int dr = dirs[i][0], dc = dirs[i][1];
        int filaMig = fila + dr;
        int colMig = columna + dc;
        int filaDest = fila + 2 * dr;
        int colDest = columna + 2 * dc;

        if (esPosicioDinsTauler(filaMig, colMig) && esPosicioDinsTauler(filaDest, colDest)) {
            if (hiHaFitxaContraria(filaMig, colMig, fitxa.getColor()) && m_tauler[filaDest][colDest].esBuida()) {

                haTrobatCaptura = true;
                capturaTrobada = true;

                Fitxa fitxaCapturada, fitxaOriginal;
                Posicio origen(fila, columna);
                Posicio mig(filaMig, colMig);
                Posicio desti(filaDest, colDest);

                simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                Moviment nouMoviment = movimentActual;
                nouMoviment.afegeixPosicio(desti);

                Fitxa& novaFitxa = m_tauler[filaDest][colDest];
                obtenirMovimentsDesDePosicio(novaFitxa, filaDest, colDest, nouMoviment, possiblesMoviments, nMoviments, capturaTrobada);

                if (nouMoviment.getNPosicions() > 1 && nMoviments < MAX_MOVIMENTS)
                    possiblesMoviments[nMoviments++] = nouMoviment;

                desfesCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);
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
                if (esPosicioDinsTauler(nf, nc) && m_tauler[nf][nc].esBuida()) {
                    Moviment m;
                    m.afegeixPosicio(Posicio(fila, columna));
                    m.afegeixPosicio(Posicio(nf, nc));

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

        while (esPosicioDinsTauler(f, c)) {
            if (m_tauler[f][c].esBuida()) {
                if (trobatContrari) {
                    // Captura vàlida
                    Posicio origen(fila, columna);
                    Posicio mig(filaContrari, columnaContrari);
                    Posicio desti(f, c);

                    Fitxa fitxaCapturada, fitxaOriginal;
                    simulaCaptura(origen, mig, desti, fitxaCapturada, fitxaOriginal);

                    Moviment nouMoviment = movimentActual;
                    nouMoviment.afegeixPosicio(desti);

                    Fitxa novaFitxa = m_tauler[f][c];
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
                if (!trobatContrari && m_tauler[f][c].getColor() != fitxa.getColor()) {
                    trobatContrari = true;
                    filaContrari = f;
                    columnaContrari = c;
                    f += dr;
                    c += dc;
                }
                else {
                    break;
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

            while (esPosicioDinsTauler(f, c) && m_tauler[f][c].esBuida()) {
                Moviment m;
                m.afegeixPosicio(Posicio(fila, columna));
                m.afegeixPosicio(Posicio(f, c));

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
            if (!m_tauler[f][c].esBuida()) {
                m_tauler[f][c] = Fitxa(); // eliminar fitxa capturada
                fitxaCapturada = true;
            }
            else {
                f += dr;
                c += dc;
            }
        }
    }
}

void Tauler::bufarSiNoHaFetMillorCaptura(const Posicio& origenSeleccionada, const Moviment& movimentFet)
{

    int millorCaptures = movimentFet.getNPosicions() - 1;
    int millorDamesCapturades = movimentFet.getNumDamesCapturades(*this);

    Posicio millorPosicio;
    bool calBufar = false;

    for (int fila = 0; fila < N_FILES; ++fila)
    {
        for (int columna = 0; columna < N_COLUMNES; ++columna)
        {
            if (fila == origenSeleccionada.getFila() && columna == origenSeleccionada.getColumna())
                continue;

            const Fitxa& fitxa = m_tauler[fila][columna];

            if (!fitxa.esBuida() && fitxa.getColor() == tornActual)
            {
                for (int i = 0; i < fitxa.getNumMoviments(); ++i)
                {
                    Moviment m = fitxa.getMoviment(i);
                    int captures = m.getNPosicions() - 1;
                    int damesCapturades = m.getNumDamesCapturades(*this);

                    // Comparació segons el criteri: més captures, i si empaten, més dames
                    if (captures > millorCaptures ||
                        (captures == millorCaptures && damesCapturades > millorDamesCapturades))
                    {
                        millorCaptures = captures;
                        millorDamesCapturades = damesCapturades;
                        millorPosicio = Posicio(fila, columna);
                        calBufar = true;
                    }
                }
            }
        }
    }

    if (calBufar)
    {
        m_tauler[millorPosicio.getFila()][millorPosicio.getColumna()] = Fitxa(); // Bufa la fitxa
    }
}

const Fitxa& Tauler::getFitxa(int fila, int columna) const
{
    return m_tauler[fila][columna];
}