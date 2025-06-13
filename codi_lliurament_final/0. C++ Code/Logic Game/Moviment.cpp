#include "Moviment.h"
#include "Fitxa.h"
#include "Tauler.h"
#include <sstream>  

Moviment::Moviment() : m_nPosicions(0) {  
    // Inicializa las posiciones a valores predeterminados
    for (int i = 0; i < MAX_POSICIONS; ++i) {
        m_posicions[i] = Posicio();
    }
}  

void Moviment::afegirPosicio(const Posicio& pos) {  
    if (m_nPosicions < MAX_POSICIONS) {  
        m_posicions[m_nPosicions++] = pos;  
    }  
}  

int Moviment::getNPosicions() const {  
    return m_nPosicions;  
}  

const Posicio& Moviment::getPosicio(int index) const {  
    return m_posicions[index];  
}  

int Moviment::getNumDamesCapturades(const Tauler& tauler) const {  
    int n = 0;  

    for (int i = 0; i < m_nPosicions - 1; ++i) {  
        Posicio p1 = m_posicions[i];
        Posicio p2 = m_posicions[i + 1];  

        int filaCap = (p1.getFila() + p2.getFila()) / 2;  
        int colCap = (p1.getColumna() + p2.getColumna()) / 2;  

        if (abs(p1.getFila() - p2.getFila()) > 1 && abs(p1.getColumna() - p2.getColumna()) > 1) {  
            const Fitxa& f = tauler.getFitxa(filaCap, colCap);  
            if (f.getTipus() == TIPUS_DAMA) {  
                ++n;  
            }  
        }  
    }  

    return n;  
}  

void Moviment::buida() {  
    m_nPosicions = 0;  
}

bool Moviment::estaBuit() const {
    return m_nPosicions < 2;
}


std::string Moviment::toString() const {  
    std::ostringstream ss;  
    for (int i = 0; i < m_nPosicions; ++i) {  
        ss << m_posicions[i].toString();  
        if (i < m_nPosicions - 1)  
            ss << " ";  
    }  
    return ss.str();  
}  

void Moviment::fromString(const std::string& linia) {  
    std::istringstream ss(linia);  
    std::string posicioStr;  
    buida();  

    while (ss >> posicioStr) {  
        Posicio p(posicioStr);  
        afegirPosicio(p);  
    }  
}  

Posicio Moviment::getDesti() const {
    Posicio desti;

    if (m_nPosicions > 0) {
        desti = m_posicions[m_nPosicions - 1];
    }

    return desti;
}

ostream& operator<<(ostream& os, const Moviment& m) {  
    for (int i = 0; i < m.getNPosicions(); ++i) {  
        os << m.getPosicio(i);  
        if (i < m.getNPosicions() - 1)  
            os << " - ";  
    }  
    return os;  
}  

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
