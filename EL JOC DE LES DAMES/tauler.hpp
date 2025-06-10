#ifndef TAULER_H
#define TAULER_H

#include <string>
#include "Fitxa.h"
#include "Posicio.h"

const int N_FILES = 8;
const int N_COLUMNES = 8;

class Tauler {

public:

    Tauler();
    void inicialitza(const std::string& nomFitxer);
    void actualitzaMovimentsValids();
    bool mouFitxa(const Posicio& origen, const Posicio& desti);
    void getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]);
    std::string toString() const;
    void inicialitzaTaulerBuit();
    void establirFitxa(const Posicio& posicio, const Fitxa& fitxa);
    void afegeixMovimentsFitxaNormal(int fila, int columna);
    void afegeixMovimentsFitxaDama(int fila, int columna);
    bool esCapturaDisponible(const Fitxa& fitxa, int fila, int columna) const;
    void obtenirMovimentsDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual,
        Moviment possiblesMoviments[], int& nMoviments, bool& capturaTrobada);
    void obtenirMovimentsDamaDesDePosicio(const Fitxa& fitxa, int fila, int columna, Moviment movimentActual,
        Moviment possiblesMoviments[], int& nMoviments);
    void eliminaFitxesCapturades(const Moviment& m);
    void bufarSiNoHaFetMillorCaptura(const Posicio& origenSeleccionada, const Moviment& movimentFet);
    const Fitxa& getFitxa(int fila, int columna) const;
    bool esPosicioDinsTauler(int fila, int columna) const;
    bool hiHaFitxaContraria(int fila, int columna, ColorFitxa colorPropi) const;
    bool esCapturaPossible(const Fitxa& fitxa, int fila, int columna) const;
    void simulaCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, Fitxa& fitxaCapturada, Fitxa& fitxaOriginal);
    void desfesCaptura(const Posicio& origen, const Posicio& mig, const Posicio& desti, const Fitxa& fitxaCapturada, const Fitxa& fitxaOriginal);

private:
    Fitxa m_tauler[N_FILES][N_COLUMNES];
    ColorFitxa tornActual;
};

#endif