#ifndef JOC_H
#define JOC_H

#include <stdio.h>
#include "info_joc.hpp"
#include "Tauler.h"
#include <string>
#include <vector>
#include "CuaMoviments.h"

using namespace std;

class Joc 
{

public:
    Joc() : guardat(false) {};
    
    bool actualitza(int mousePosX, int mousePosY, bool mouseStatus); 
    void inicialitza(ModeJoc mode, const string& nomFitxerTauler, const string& nomFitxerMoviments);
    void finalitza();
    
private:
    // Estat general
    ModeJoc m_mode;
    bool m_partidaAcabada;
    ColorFitxa m_guanyador;
    ColorFitxa m_tornActual;

    // Fitxer per guardar/reproduir
    std::string m_nomFitxerMoviments;

    // Tauler i moviments
    Tauler m_tauler;
    CuaMoviments m_cua;

    // Selecció de fitxa i moviments vàlids
    bool m_hiHaFitxaSeleccionada;
    bool guardat;
    Posicio m_fichaSeleccionada;
    vector<Posicio> m_posicionsValides;
    std::string m_missatgeTemporal;  // missatge que persistirà

    // Mètodes auxiliars
    bool esClicEnCasella(int mouseX, int mouseY) const;
    Posicio obtenirCasellaClicada(int mouseX, int mouseY) const;
    bool esClicEnPosicioValida(const Posicio& clic) const;

    void mostraInformacioModeITorn() const;
    void mostraGuanyador() const;
    void mostraMissatge(const std::string& text);

    void actualitzaEstatPartida();

};

#endif 
