#include "Fitxa.h"
#include "GraphicManager.h"
//Gestionar l’estat del tauler i de les fitxes, incloent moviments, captures i promocions.
// Converteix una fitxa normal en dama
void Fitxa::convertirEnDama()
{
    if (m_tipus == TIPUS_NORMAL)
    {
        m_tipus = TIPUS_DAMA; // només si és normal, la canviem a dama
    }
}

// Afegeix un moviment vàlid a la fitxa (si no supera el màxim)
void Fitxa::afegeixMovimentValid(const Moviment& m)
{
    if (m_nMoviments < MAX_MOVIMENTS) // evitem sobrepassar la capacitat del vector
    {
        m_movimentsValids[m_nMoviments++] = m; // afegeix el moviment i incrementa el comptador
    }
}

// Elimina tots els moviments vàlids (neteja el vector)
void Fitxa::netejaMoviments()
{
    m_nMoviments = 0; // simplement posem a 0 el comptador (no cal esborrar manualment)
}

// Mostra per pantalla tots els moviments vàlids de la fitxa
void Fitxa::mostraMoviments() const {
    for (int i = 0; i < m_nMoviments; ++i) {
        std::cout << m_movimentsValids[i] << std::endl;
    }
}

// Dibuixa gràficament la fitxa a la posició (x, y)
void Fitxa::visualitza(int x, int y) const {
    IMAGE_NAME idGrafic = GRAFIC_FITXA_BLANCA; // valor per defecte per evitar warnings
    bool pintar = false;

    // Només si la fitxa no és buida la pintem
    if (!esBuida()) {
        // Assignem el gràfic corresponent segons el color i el tipus
        if (m_color == COLOR_BLANC && m_tipus == TIPUS_NORMAL)
            idGrafic = GRAFIC_FITXA_BLANCA, pintar = true;
        else if (m_color == COLOR_NEGRE && m_tipus == TIPUS_NORMAL)
            idGrafic = GRAFIC_FITXA_NEGRA, pintar = true;
        else if (m_color == COLOR_BLANC && m_tipus == TIPUS_DAMA)
            idGrafic = GRAFIC_DAMA_BLANCA, pintar = true;
        else if (m_color == COLOR_NEGRE && m_tipus == TIPUS_DAMA)
            idGrafic = GRAFIC_DAMA_NEGRA, pintar = true;
    }

    // Dibuixa la fitxa amb el gràfic escollit
    if (pintar) {
        GraphicManager::getInstance()->drawSprite(idGrafic, x, y);
    }
}

