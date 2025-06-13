#include "Posicio.h"
#include "Tauler.h"

Posicio::Posicio(const string& posicio) : m_posicio(posicio) {
    if (posicio.size() >= 2) {
        char colChar = tolower(posicio[0]);
        char filaChar = posicio[1];

        if (colChar >= 'a' && colChar <= 'h' && filaChar >= '1' && filaChar <= '8') {
            m_columna = colChar - 'a';
            m_fila = 8 - (filaChar - '0');
        }
        else {
            m_fila = m_columna = -1;
        }
    }
    else {
        m_fila = m_columna = -1;
    }
}

bool Posicio::posicioValida() const {
    return m_fila >= 0 && m_fila < 8 && m_columna >= 0 && m_columna < 8;
}

bool Posicio::operator==(const Posicio& pos) const {
    return m_fila == pos.m_fila && m_columna == pos.m_columna;
}

std::string Posicio::toString() const {
    char lletraCol = 'a' + m_columna;
    char numFila = '1' + (7 - m_fila); // fila 0 ? 8, fila 7 ? 1
    return std::string() + lletraCol + numFila;
}


std::ostream& operator<<(std::ostream& os, const Posicio& p) {
    char columnaChar = 'a' + p.getColumna();
    char filaChar = '1' + (7 - p.getFila());
    os << columnaChar << filaChar;
    return os;
}

