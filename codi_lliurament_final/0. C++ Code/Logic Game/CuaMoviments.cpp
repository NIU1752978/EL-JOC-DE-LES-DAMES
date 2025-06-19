#include "CuaMoviments.h"
#include <fstream>
//Controlar, desar i recuperar els moviments realitzats durant la partida.
// Constructor: inicialitza els punters a nullptr
CuaMoviments::CuaMoviments() : m_inici(nullptr), m_final(nullptr) {}

// Destructor: allibera la memòria buidant la cua
CuaMoviments::~CuaMoviments() {
    buida();
}

// Afegeix un moviment al final de la cua
void CuaMoviments::afegeixMoviment(const Moviment& m)
{
    Node* nou = new Node{ m, nullptr }; // crea un nou node amb el moviment

    if (m_final != nullptr)
        m_final->seguent = nou; // enllaça l'últim node amb el nou
    else
        m_inici = nou; // si la cua estava buida, aquest és el primer node

    m_final = nou; // actualitza el final de la cua
}

// Extreu i retorna el primer moviment de la cua
Moviment CuaMoviments::getSeguentMoviment() {
    Moviment resultat;

    if (!esBuida()) {
        Node* primer = m_inici;          // agafa el primer node
        resultat = primer->moviment;     // guarda el moviment a retornar
        m_inici = m_inici->seguent;      // avança l'inici

        if (m_inici == nullptr)
            m_final = nullptr; // si ara està buida, també anul·la m_final

        delete primer; // allibera la memòria del node extret
    }

    return resultat; // retorna el moviment (buit si la cua estava buida)
}

// Retorna cert si la cua està buida
bool CuaMoviments::esBuida() const {
    return m_inici == nullptr;
}

// Buida completament la cua (allibera tota la memòria)
void CuaMoviments::buida() {
    while (!esBuida()) {
        getSeguentMoviment(); // elimina el primer fins que estigui buida
    }
}

// Desa tots els moviments de la cua en un fitxer de text
void CuaMoviments::guardaEnFitxer(const std::string& nomFitxer) const {
    std::ofstream fitxer(nomFitxer); // obre el fitxer per escriure
    Node* actual = m_inici;

    while (actual != nullptr) {
        fitxer << actual->moviment.toString() << std::endl; // escriu cada moviment
        actual = actual->seguent; // passa al següent
    }
}

// Carrega els moviments d’un fitxer i els afegeix a la cua
void CuaMoviments::carregaDeFitxer(const std::string& nomFitxer)
{
    std::ifstream fitxer(nomFitxer);
    bool fitxerObert = fitxer.is_open();

    if (fitxerObert) {
        Moviment m;
        std::string linia;

        while (std::getline(fitxer, linia)) {
            m.fromString(linia);      // reconstrueix el moviment des de la línia
            afegeixMoviment(m);       // afegeix-lo a la cua
        }

        fitxer.close(); // tanca el fitxer
    }
}
