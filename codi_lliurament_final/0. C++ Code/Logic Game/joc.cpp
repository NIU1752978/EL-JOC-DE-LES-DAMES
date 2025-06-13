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
        m_missatgeTemporal = "";  // esborra missatge temporal en fer clic

    // Dibuixa fons i tauler
    GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    // Dibuixa totes les fitxes
    m_tauler.visualitza();

    // Mostra les caselles vàlides si hi ha fitxa seleccionada
    if (m_hiHaFitxaSeleccionada) {
        for (size_t i = 0; i < m_posicionsValides.size(); ++i) {
            int x = POS_X_TAULER + CASELLA_INICIAL_X + m_posicionsValides[i].getColumna() * AMPLADA_CASELLA;
            int y = POS_Y_TAULER + CASELLA_INICIAL_Y + m_posicionsValides[i].getFila() * ALCADA_CASELLA;
            GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);
        }
    }

    // Mostra el mode i el torn actual
    mostraInformacioModeITorn();

    // Si la partida s'ha acabat, mostra guanyador i desa el fitxer si cal
    if (m_partidaAcabada) {
        mostraGuanyador();

        if (!guardat) {
            finalitza();  // desa els moviments
            guardat = true;
        }

        resultat = true;
    }

    // Si s'ha fet clic dins del tauler
    if (mouseStatus && esClicEnCasella(mousePosX, mousePosY)) {
        Posicio clic = obtenirCasellaClicada(mousePosX, mousePosY);

        if (m_mode == MODE_JOC_NORMAL) {
            // Si hi ha fitxa seleccionada i el clic és en una posició vàlida → mou
            if (m_hiHaFitxaSeleccionada && esClicEnPosicioValida(clic)) {
                Moviment moviment = m_tauler.mouFitxa(m_fichaSeleccionada, clic, m_tornActual);

                if (!moviment.estaBuit()) {
                    m_cua.afegeixMoviment(moviment);  // guarda el moviment
                    actualitzaEstatPartida();         // actualitza torn o guanyador
                }

                m_hiHaFitxaSeleccionada = false;
            }
            // Si clic sobre fitxa pròpia → selecciona
            else if (m_tauler.getColorFitxa(clic) == m_tornActual) {
                m_fichaSeleccionada = clic;
                m_tauler.actualitzaMovimentsValids();
                m_tauler.getPosicionsPossibles(clic, m_posicionsValides);
                m_hiHaFitxaSeleccionada = true;
            }
            // Clic fora de tot → deselecciona
            else {
                m_hiHaFitxaSeleccionada = false;
            }
        }

        // Mode Replay: avança al següent moviment si n'hi ha
        else if (m_mode == MODE_JOC_REPLAY) {
            if (!m_cua.esBuida()) {
                Moviment moviment = m_cua.getSeguentMoviment();
                m_tauler.executaMoviment(moviment);

                // Canvi de torn
                if (m_tornActual == COLOR_BLANC)
                    m_tornActual = COLOR_NEGRE;
                else
                    m_tornActual = COLOR_BLANC;
            }
            else {
                m_partidaAcabada = true;
            }
        }
    }

    // Si hi ha algun missatge, el mostra
    if (!m_missatgeTemporal.empty()) {
        GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 50, 700, 1.0, m_missatgeTemporal);
    }

    return resultat;  // indica si la partida ha acabat
}

void Joc::inicialitza(ModeJoc mode, const std::string& nomFitxerTauler, const std::string& nomFitxerMoviments)
{
    m_mode = mode;
    m_tauler.inicialitza("tauler_inicial.txt");  // carrega tauler base

    m_hiHaFitxaSeleccionada = false;
    m_partidaAcabada = false;
    m_guanyador = CAP_GUANYADOR;
    m_tornActual = COLOR_BLANC;

    if (mode == MODE_JOC_NORMAL) {
        m_cua.buida();  // cua buida per anar afegint jugades
        m_nomFitxerMoviments = nomFitxerMoviments;
    }
    else if (mode == MODE_JOC_REPLAY) {
        m_nomFitxerMoviments = nomFitxerMoviments;
        m_cua.carregaDeFitxer(m_nomFitxerMoviments);  // carrega jugades
    }
}

void Joc::finalitza()
{
    if (m_mode == MODE_JOC_NORMAL) {
        m_cua.guardaEnFitxer(m_nomFitxerMoviments);  // desa jugades
    }
}

bool Joc::esClicEnCasella(int mouseX, int mouseY) const
{
    return (mouseX >= POS_X_TAULER + CASELLA_INICIAL_X &&
        mouseX < POS_X_TAULER + CASELLA_INICIAL_X + N_COLUMNES * AMPLADA_CASELLA &&
        mouseY >= POS_Y_TAULER + CASELLA_INICIAL_Y &&
        mouseY < POS_Y_TAULER + CASELLA_INICIAL_Y + N_FILES * ALCADA_CASELLA); //Retorna true si el clic ha sigut dins del tauler.
}

Posicio Joc::obtenirCasellaClicada(int mouseX, int mouseY) const
{
    int fila = (mouseY - POS_Y_TAULER - CASELLA_INICIAL_Y) / ALCADA_CASELLA;
    int columna = (mouseX - POS_X_TAULER - CASELLA_INICIAL_X) / AMPLADA_CASELLA;
    return Posicio(fila, columna); //Tradueix la posició del clic en coordenades de casella (fila, columna).
}

bool Joc::esClicEnPosicioValida(const Posicio& clic) const
{
    bool esValid = false;

    for (const Posicio& p : m_posicionsValides) {
        if (p == clic) {
            esValid = true;
        }
    }

	return esValid; //Retorna true si el clic ha sigut en una posició vàlida per moure la fitxa seleccionada.
}

void Joc::mostraInformacioModeITorn() const
{
    std::string text;

    // Determina el mode de joc
    if (m_mode == MODE_JOC_NORMAL)
    {
        text = "Mode: Joc Normal";
    }
    else
    {
        text = "Mode: Reproduccio";
    }

    // Afegeix informació del torn
    if (m_mode == MODE_JOC_NORMAL && !m_partidaAcabada)
    {
        text += " | Torn: ";
        if (m_tornActual == COLOR_BLANC)
        {
            text += "Blanc";
        }
        else
        {
            text += "Negre";
        }
    }
	else if (m_mode == MODE_JOC_REPLAY) // En mode replay, mostra el torn actual si hi ha moviments
    {
        if (!m_cua.esBuida())
        {
            text += " | Torn: ";
            if (m_tornActual == COLOR_BLANC)
            {
                text += "Blanc";
            }
            else
            {
                text += "Negre";
            }
        }
        else
        {
            text += " | No hi ha mes moviments";
        }
    }

    // Mostra el text per pantalla
    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 50, 30, 1.0, text);
}

void Joc::mostraGuanyador() const
{
    std::string text;
    bool mostrar = false;

	if (m_guanyador == COLOR_BLANC) {// Si el guanyador és blanc
        text = "Guanyador: Blanc";
        mostrar = true;
    }
	else if (m_guanyador == COLOR_NEGRE) { // Si el guanyador és negre
        text = "Guanyador: Negre";
        mostrar = true;
    }

	if (mostrar) { // Si hi ha guanyador, mostra el missatge
        GraphicManager::getInstance()->drawFont(FONT_GREEN_30, 50, 750, 1.0, text);
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
        // Comprova si cada jugador pot fer moviments
        bool potMoureNegre = m_tauler.teMoviments(ColorFitxa::COLOR_NEGRE);
        bool potMoureBlanc = m_tauler.teMoviments(ColorFitxa::COLOR_BLANC);

        // Cap jugador pot moure
        if (!potMoureBlanc && !potMoureNegre) {
            m_guanyador = CAP_GUANYADOR;
            m_partidaAcabada = true;
        }
        // Només negre no pot moure → guanya blanc
        else if (!potMoureNegre) {
            m_guanyador = COLOR_BLANC;
            m_partidaAcabada = true;
        }
        // Només blanc no pot moure → guanya negre
        else if (!potMoureBlanc) {
            m_guanyador = COLOR_NEGRE;
            m_partidaAcabada = true;
        }
        // Si tots dos poden moure → canvia el torn
        else {
            if (m_tornActual == COLOR_BLANC) {
                m_tornActual = COLOR_NEGRE;
            }
            else {
                m_tornActual = COLOR_BLANC;
            }
        }
    }
}
