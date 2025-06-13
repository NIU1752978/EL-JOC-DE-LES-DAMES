#include "Fitxa.h"
#include "GraphicManager.h"

void Fitxa::convertirEnDama() //peça normal a dama
{
    if (m_tipus == TIPUS_NORMAL)
    {
        m_tipus = TIPUS_DAMA;
    }
}

void Fitxa::afegeixMovimentValid(const Moviment& m) //afegim moviments valids al vector
{
    if (m_nMoviments < MAX_MOVIMENTS)
    {
        m_movimentsValids[m_nMoviments++] = m;
    }
}

void Fitxa::netejaMoviments() //netegem els moviments
{
    m_nMoviments = 0;
}

void Fitxa::mostraMoviments() const {
    for (int i = 0; i < m_nMoviments; ++i) {
        std::cout << m_movimentsValids[i] << std::endl;
    }
}

void Fitxa::visualitza(int x, int y) const {
    IMAGE_NAME idGrafic = GRAFIC_FITXA_BLANCA; // valor per defecte qualsevol
    bool pintar = false;

    if (!esBuida()) {
        if (m_color == COLOR_BLANC && m_tipus == TIPUS_NORMAL)
            idGrafic = GRAFIC_FITXA_BLANCA, pintar = true;
        else if (m_color == COLOR_NEGRE && m_tipus == TIPUS_NORMAL)
            idGrafic = GRAFIC_FITXA_NEGRA, pintar = true;
        else if (m_color == COLOR_BLANC && m_tipus == TIPUS_DAMA)
            idGrafic = GRAFIC_DAMA_BLANCA, pintar = true;
        else if (m_color == COLOR_NEGRE && m_tipus == TIPUS_DAMA)
            idGrafic = GRAFIC_DAMA_NEGRA, pintar = true;
    }

    if (pintar) {
        GraphicManager::getInstance()->drawSprite(idGrafic, x, y);
    }

}

