#ifndef TAULER_H
#define TAULER_H

#include <string>
#include <vector>
#include "Fitxa.h"
#include "Posicio.h"
#include "Moviment.h"

const int N_FILES = 8;
const int N_COLUMNES = 8;

using namespace std;
class Moviment;

class Tauler {

public:

    Tauler();
    ~Tauler();
    void inicialitza(const std::string& nomFitxer);
    void actualitzaMovimentsValids();
    Moviment mouFitxa(const Posicio& origen, const Posicio& desti, ColorFitxa tornActual);
    void getPosicionsPossibles(const Posicio& origen, vector<Posicio>& posicionsPossibles) const;
    std::string toString() const;
    void inicialitzaTaulerBuit();
    void establirFitxa(const Posicio& posicio, Fitxa* fitxa);
    void afegeixMovimentsFitxaNormal(int fila, int columna);
    void afegeixMovimentsFitxaDama(int fila, int columna);
    bool esCapturaDisponible(const Fitxa& fitxa, int fila, int columna) const;
    void obtenirMovimentsDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual,
        Moviment possiblesMoviments[], int& nMoviments, bool& capturaTrobada);
    void obtenirMovimentsDamaDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual,
        Moviment possiblesMoviments[], int& nMoviments);
    void eliminaFitxesCapturades(const Moviment& m);
    void bufarSiNoHaFetMillorCaptura(const Posicio& origenSeleccionada, const Moviment& movimentFet, ColorFitxa tornActual);
    const Fitxa& getFitxa(int fila, int columna) const;
    bool esPosicioDinsTauler(int fila, int columna) const;
    bool hiHaFitxaContraria(int fila, int columna, ColorFitxa colorPropi) const;
    bool esCapturaPossible(const Fitxa& fitxa, int fila, int columna) const;
    void simulaCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, Fitxa& fitxaCapturada, Fitxa& fitxaOriginal);
    void desfesCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, const Fitxa& fitxaCapturada, const Fitxa& fitxaOriginal);
    void visualitza() const;
    ColorFitxa getColorFitxa(const Posicio& pos) const;
    void executaMoviment(const Moviment& moviment);
    bool teMoviments(ColorFitxa color) const;
    bool esMovimentCapturaReal(const Moviment& m) const;

private:
    Fitxa* m_tauler[N_FILES][N_COLUMNES];
};

#endif