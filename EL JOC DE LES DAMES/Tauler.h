#include <iostream>
#include <string>
#include "Posicio.h"
#include "Fitxa.h"

#define N_FILES 8
#define N_COLUMNES 8

using namespace std; 

class Tauler
{
public:
	Tauler() {}

	void inicialitza(const string& nomFitxer);
	void actualitzaMovimentsValids();
	bool mouFitxa(const Posicio& origen, const Posicio& desti);
	string toString() const;
	void getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]);
	void establirFitxa(const Posicio& posicio, const Fitxa& fitxa);
	void obtenirPosicionsValides(const Posicio& origen, Posicio posicionsValides[], int& nPosicionsValides);
	bool esCaptura(const Posicio& origen, const Posicio& desti);

private:
	Fitxa m_tauler[N_FILES][N_COLUMNES];

	void inicialitzaTaulerVuit();
	void carregaFitxes(const string& nomFitxer);
};

