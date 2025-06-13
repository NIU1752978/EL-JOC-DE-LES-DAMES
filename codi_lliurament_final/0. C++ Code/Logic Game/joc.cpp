//
//  CurrentGame.cpp
//  LearnChess
//
//  Created by Enric Vergara on 21/2/22.
//

#include "joc.hpp"
#include <iostream>
#include <fstream>
#include "GraphicManager.h"

bool Joc::actualitza(int mousePosX, int mousePosY, bool mouseStatus)
{
    bool resultat = false;

    if (mouseStatus)
        m_missatgeTemporal = "";  // neteja el missatge al següent clic

    // 🔁 Dibuixar fons i tauler
    GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    // 🔁 Dibuixar totes les fitxes
    m_tauler.visualitza();

    // 🔁 Mostrar posicions vàlides (si n’hi ha)
    if (m_hiHaFitxaSeleccionada) {
        for (size_t i = 0; i < m_posicionsValides.size(); ++i) {
            int x = POS_X_TAULER + CASELLA_INICIAL_X + m_posicionsValides[i].getColumna() * AMPLADA_CASELLA;
            int y = POS_Y_TAULER + CASELLA_INICIAL_Y + m_posicionsValides[i].getFila() * ALCADA_CASELLA;
            GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);
        }
    }

    // 🔁 Mostrar info de mode i torn
    mostraInformacioModeITorn();

    // 🔁 Si la partida s'ha acabat, mostrar guanyador i sortir
    if (m_partidaAcabada) {
        mostraGuanyador();

        if (!guardat) {
            finalitza();
            guardat = true;
        }

        resultat = true;
    }

    // 🔁 Si hi ha clic i és dins del tauler:
    if (mouseStatus && esClicEnCasella(mousePosX, mousePosY)) {
        Posicio clic = obtenirCasellaClicada(mousePosX, mousePosY);

        if (m_mode == MODE_JOC_NORMAL) {
            // 🔁 Primer: intentar fer moviment si clic és posició vàlida
            if (m_hiHaFitxaSeleccionada && esClicEnPosicioValida(clic)) {
                Moviment moviment = m_tauler.mouFitxa(m_fichaSeleccionada, clic, m_tornActual);

                if (!moviment.estaBuit()) {
                    m_cua.afegeixMoviment(moviment);
                    actualitzaEstatPartida();
                }

                m_hiHaFitxaSeleccionada = false;
            }
            // 🔁 Si es clica sobre una fitxa pròpia → canviar selecció
            else if (m_tauler.getColorFitxa(clic) == m_tornActual) {
                m_fichaSeleccionada = clic;
                m_tauler.actualitzaMovimentsValids();
                m_tauler.getPosicionsPossibles(clic, m_posicionsValides);
                m_hiHaFitxaSeleccionada = true;
            }
            // 🔁 Si es clica fora de tot → cancel·la selecció
            else {
                m_hiHaFitxaSeleccionada = false;
            }
        }

        else if (m_mode == MODE_JOC_REPLAY) {
            if (!m_cua.esBuida()) {
                Moviment moviment = m_cua.getSeguentMoviment();
                m_tauler.executaMoviment(moviment);
                m_tornActual = (m_tornActual == COLOR_BLANC) ? COLOR_NEGRE : COLOR_BLANC;
            }
            else {
                mostraMissatge("No hi ha més moviments per reproduir.");
                m_partidaAcabada = true;
            }
        }
    }

    if (!m_missatgeTemporal.empty()) {
        GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 50, 700, 1.0, m_missatgeTemporal);
    }

    return resultat;
}

void Joc::inicialitza(ModeJoc mode, const std::string& nomFitxerTauler, const std::string& nomFitxerMoviments)
{
    m_mode = mode;
    m_tauler.inicialitza("tauler_inicial.txt");  // carrega el tauler

    m_hiHaFitxaSeleccionada = false;
    m_partidaAcabada = false;
    m_guanyador = CAP_GUANYADOR;
    m_tornActual = COLOR_BLANC;  // o el que toqui

    if (mode == MODE_JOC_NORMAL)
    {
        m_cua.buida();  // comença una cua buida per anar afegint moviments
        m_nomFitxerMoviments = nomFitxerMoviments;  // guardar-lo per al final
    }
    else if (mode == MODE_JOC_REPLAY)
    {
        m_nomFitxerMoviments = nomFitxerMoviments; // assigna el nom que reps
        m_cua.carregaDeFitxer(m_nomFitxerMoviments); // fes servir aquest
    }

}

void Joc::finalitza()
{
    if (m_mode == MODE_JOC_NORMAL)
    {
        m_cua.guardaEnFitxer(m_nomFitxerMoviments);
    }
}

bool Joc::esClicEnCasella(int mouseX, int mouseY) const
{
    return (mouseX >= POS_X_TAULER + CASELLA_INICIAL_X &&
        mouseX < POS_X_TAULER + CASELLA_INICIAL_X + N_COLUMNES * AMPLADA_CASELLA &&
        mouseY >= POS_Y_TAULER + CASELLA_INICIAL_Y &&
        mouseY < POS_Y_TAULER + CASELLA_INICIAL_Y + N_FILES * ALCADA_CASELLA);
}

Posicio Joc::obtenirCasellaClicada(int mouseX, int mouseY) const
{
    int fila = (mouseY - POS_Y_TAULER - CASELLA_INICIAL_Y) / ALCADA_CASELLA;
    int columna = (mouseX - POS_X_TAULER - CASELLA_INICIAL_X) / AMPLADA_CASELLA;
    return Posicio(fila, columna);
}

bool Joc::esClicEnPosicioValida(const Posicio& clic) const
{
    bool esValid = false;

    for (const Posicio& p : m_posicionsValides) {
        if (p == clic) {
            esValid = true;
        }
    }

    return esValid;
}

void Joc::mostraInformacioModeITorn() const
{
    std::string text = (m_mode == MODE_JOC_NORMAL)
        ? "Mode: Joc Normal"
        : "Mode: Reproduccio";

    if (m_mode == MODE_JOC_NORMAL && !m_partidaAcabada) {
        text += " | Torn: ";
        text += (m_tornActual == COLOR_BLANC) ? "Blanc" : "Negre";
    }
    else if (m_mode == MODE_JOC_REPLAY) {
        if (!m_cua.esBuida()) {
            text += " | Torn: ";
            text += (m_tornActual == COLOR_BLANC) ? "Blanc" : "Negre";
        }
        else {
            text += " | No hi ha mes moviments";
        }
    }

    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 50, 30, 1.0, text);
}

void Joc::mostraGuanyador() const
{
    std::string text;
    bool mostrar = false;

    if (m_guanyador == COLOR_BLANC) {
        text = "Guanyador: Blanc";
        mostrar = true;
    }
    else if (m_guanyador == COLOR_NEGRE) {
        text = "Guanyador: Negre";
        mostrar = true;
    }

    if (mostrar) {
        GraphicManager::getInstance()->drawFont(FONT_GREEN_30, 50, 700, 1.0, text);
    }
}

void Joc::mostraMissatge(const std::string& text)
{
    m_missatgeTemporal = text;  // guarda el missatge per dibuixar-lo després
}

void Joc::actualitzaEstatPartida()
{
    bool esReplay = (m_mode == MODE_JOC_REPLAY);

    if (!esReplay) {
        bool potMoureNegre = m_tauler.teMoviments(ColorFitxa::COLOR_NEGRE);
        bool potMoureBlanc = m_tauler.teMoviments(ColorFitxa::COLOR_BLANC);

        if (!potMoureBlanc && !potMoureNegre) {
            m_guanyador = CAP_GUANYADOR;
            m_partidaAcabada = true;
        }
        else if (!potMoureNegre) {
            m_guanyador = COLOR_BLANC;
            m_partidaAcabada = true;
        }
        else if (!potMoureBlanc) {
            m_guanyador = COLOR_NEGRE;
            m_partidaAcabada = true;
        }
        else {
            m_tornActual = (m_tornActual == COLOR_BLANC) ? COLOR_NEGRE : COLOR_BLANC;
        }
    }
}
