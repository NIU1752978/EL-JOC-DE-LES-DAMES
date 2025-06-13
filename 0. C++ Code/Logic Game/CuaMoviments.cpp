#include "CuaMoviments.h"
#include <fstream>

CuaMoviments::CuaMoviments() : m_inici(nullptr), m_final(nullptr) {}

CuaMoviments::~CuaMoviments() {
    buida();
}

void CuaMoviments::afegeixMoviment(const Moviment& m) {
    Node* nou = new Node{ m, nullptr };
    if (m_final != nullptr) m_final->seguent = nou;
    else m_inici = nou;
    m_final = nou;
}

Moviment CuaMoviments::getSeguentMoviment() {
    Moviment resultat;

    if (!esBuida()) {
        Node* primer = m_inici;
        resultat = primer->moviment;
        m_inici = m_inici->seguent;
        if (m_inici == nullptr) {
            m_final = nullptr;
        }
        delete primer;
    }

    return resultat;
}


bool CuaMoviments::esBuida() const {
    return m_inici == nullptr;
}

void CuaMoviments::buida() {
    while (!esBuida()) {
        getSeguentMoviment();
    }
}

void CuaMoviments::guardaEnFitxer(const std::string& nomFitxer) const {
    std::ofstream fitxer(nomFitxer);
    Node* actual = m_inici;
    while (actual != nullptr) {
        fitxer << actual->moviment.toString() << std::endl;
        actual = actual->seguent;
    }
}

void CuaMoviments::carregaDeFitxer(const std::string& nomFitxer)
{
    std::ifstream fitxer(nomFitxer);
    bool fitxerObert = fitxer.is_open();

    if (fitxerObert) {
        Moviment m;
        std::string linia;

        while (std::getline(fitxer, linia)) {
            m.fromString(linia);      // <-- reconstrueix el moviment
            afegeixMoviment(m);       // <-- afegeix-lo a la cua
        }

        fitxer.close();
    }
}


