#ifndef CUA_MOVIMENTS_H
#define CUA_MOVIMENTS_H

#include <string>
#include <iostream>
#include <fstream>
#include "Moviment.h"

using namespace std;

struct Node {
    Moviment moviment;
    Node* seguent;
};

class CuaMoviments {
public:
    CuaMoviments();
    ~CuaMoviments();

    void afegeixMoviment(const Moviment& m);
    Moviment getSeguentMoviment();
    bool esBuida() const;
    void buida();

    void guardaEnFitxer(const std::string& nomFitxer) const;
    void carregaDeFitxer(const std::string& nomFitxer);

private:
    Node* m_inici;
    Node* m_final;
};

#endif