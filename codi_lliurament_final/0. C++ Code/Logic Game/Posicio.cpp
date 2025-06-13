#include "Posicio.h"
#include "Tauler.h"

// Constructor que rep una cadena com "e3" i la transforma en fila i columna
Posicio::Posicio(const string& posicio) : m_posicio(posicio) {
    if (posicio.size() >= 2) {  // Comprova que la posició té almenys dues lletres
        char colChar = tolower(posicio[0]);  // Columna (lletra, ex: 'e')
        char filaChar = posicio[1];          // Fila (número, ex: '3')

        // Comprova que la lletra estigui entre 'a' i 'h' i el número entre '1' i '8'
        if (colChar >= 'a' && colChar <= 'h' && filaChar >= '1' && filaChar <= '8') {
            m_columna = colChar - 'a';             // 'a' = 0, 'b' = 1, ..., 'h' = 7
            m_fila = 8 - (filaChar - '0');         // '1' = fila 7, '8' = fila 0 (inverteix l'eix vertical)
        }
        else {
            m_fila = m_columna = -1;  // Si la posició és invàlida, marca-la com a tal
        }
    }
    else {
        m_fila = m_columna = -1;  // Si la string no és vàlida, marca com a invàlida
    }
}

// Comprova si la posició és dins del tauler (8x8)
bool Posicio::posicioValida() const {
    return m_fila >= 0 && m_fila < 8 && m_columna >= 0 && m_columna < 8;
}

// Sobrecàrrega de l'operador == per comparar dues posicions
bool Posicio::operator==(const Posicio& pos) const {
    return m_fila == pos.m_fila && m_columna == pos.m_columna;
}

// Converteix una posició interna (fila/columna) a string tipus "e3"
std::string Posicio::toString() const {
    char lletraCol = 'a' + m_columna;          // columna 0 ? 'a', columna 7 ? 'h'
    char numFila = '1' + (7 - m_fila);         // fila 0 ? '8', fila 7 ? '1'
    return std::string() + lletraCol + numFila;
}

// Sobrecàrrega de l'operador << per imprimir una posició per consola
std::ostream& operator<<(std::ostream& os, const Posicio& p) {
    char columnaChar = 'a' + p.getColumna();   // transforma columna a lletra
    char filaChar = '1' + (7 - p.getFila());   // transforma fila a número
    os << columnaChar << filaChar;
    return os;
}
