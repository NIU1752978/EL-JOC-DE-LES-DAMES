#include "Moviment.h"
#include "Fitxa.h"
#include "Tauler.h"
#include <sstream>  

// Constructor per defecte
Moviment::Moviment() : m_nPosicions(0) {
    // Inicialitza totes les posicions amb posicions buides
    for (int i = 0; i < MAX_POSICIONS; ++i) {
        m_posicions[i] = Posicio();
    }
}

// Afegeix una nova posició al moviment si hi ha espai
void Moviment::afegirPosicio(const Posicio& pos) {
    if (m_nPosicions < MAX_POSICIONS) {
        m_posicions[m_nPosicions++] = pos;
    }
}

// Retorna el nombre total de posicions en el moviment
int Moviment::getNPosicions() const {
    return m_nPosicions;
}

// Retorna la posició en la posició index
const Posicio& Moviment::getPosicio(int index) const {
    return m_posicions[index];
}

// Calcula quantes dames s'han capturat en aquest moviment
int Moviment::getNumDamesCapturades(const Tauler& tauler) const {
    int n = 0;

    // Compara cada parella consecutiva de posicions
    for (int i = 0; i < m_nPosicions - 1; ++i) {
        Posicio p1 = m_posicions[i];
        Posicio p2 = m_posicions[i + 1];

        // Calcula la posició intermitja entre p1 i p2
        int filaCap = (p1.getFila() + p2.getFila()) / 2;
        int colCap = (p1.getColumna() + p2.getColumna()) / 2;

        // Si el moviment és una captura (salta 2 o més)
        if (abs(p1.getFila() - p2.getFila()) > 1 && abs(p1.getColumna() - p2.getColumna()) > 1) {
            const Fitxa& f = tauler.getFitxa(filaCap, colCap);
            if (f.getTipus() == TIPUS_DAMA) {
                ++n;  // Incrementa si la fitxa capturada és una dama
            }
        }
    }

    return n;
}

// Esborra totes les posicions del moviment
void Moviment::buida() {
    m_nPosicions = 0;
}

// Comprova si el moviment està buit (menys de 2 posicions)
bool Moviment::estaBuit() const {
    return m_nPosicions < 2;
}

// Converteix el moviment a string per desar-lo
std::string Moviment::toString() const {
    std::ostringstream ss;
    for (int i = 0; i < m_nPosicions; ++i) {
        ss << m_posicions[i].toString();
        if (i < m_nPosicions - 1)
            ss << " ";  // separa les posicions amb espais
    }
    return ss.str();
}

// Construeix un moviment a partir d'una línia llegida del fitxer
void Moviment::fromString(const std::string& linia) {
    std::istringstream ss(linia);
    std::string posicioStr;
    buida();  // esborra primer qualsevol moviment anterior

    while (ss >> posicioStr) {
        Posicio p(posicioStr);  // crea la posició a partir del string
        afegirPosicio(p);       // afegeix-la al moviment
    }
}

// Retorna la posició final (destí) del moviment
Posicio Moviment::getDesti() const {
    Posicio desti;

    if (m_nPosicions > 0) {
        desti = m_posicions[m_nPosicions - 1];
    }

    return desti;
}

// Sobrecàrrega de l'operador << per imprimir un moviment per pantalla
ostream& operator<<(ostream& os, const Moviment& m) {
    for (int i = 0; i < m.getNPosicions(); ++i) {
        os << m.getPosicio(i);
        if (i < m.getNPosicions() - 1)
            os << " - ";  // separació visual entre posicions
    }
    return os;
}

// Sobrecàrrega de l'operador >> per llegir un moviment des de l’entrada
std::istream& operator>>(std::istream& input, Moviment& m) {
    std::string origenStr, destiStr;
    if (input >> origenStr >> destiStr) {
        Posicio origen(origenStr);
        Posicio desti(destiStr);
        m.afegirPosicio(origen);
        m.afegirPosicio(desti);
    }
    return input;
}
