//
//  GraphicManager.cpp
//  MP_Practica
//
//  Created by Enric Vergara on 13/03/2021.
//  Copyright © 2021 CompiledCreations. All rights reserved.
//

#include "GraphicManager.h"
#include <iostream>

// Inicialització del punter a la instància singleton a NULL
GraphicManager* GraphicManager::instance = NULL;

// Constructor del GraphicManager
GraphicManager::GraphicManager()
// Inicialitza les fonts amb color i mida
    : m_oFontWhite_30(getFontPath(FONT_WHITE_30).c_str(), 30, NFont::Color(255, 255, 255, 255))
    , m_oFontRed_30(getFontPath(FONT_RED_30).c_str(), 30, NFont::Color(255, 0, 0, 255))
    , m_oFontGreen_30(getFontPath(FONT_GREEN_30).c_str(), 30, NFont::Color(0, 255, 0, 255))
{
    // Crea tots els sprites gràfics definits a l'enumeració IMAGE_NAME
    for (int i = 0; i < GRAFIC_NUM_MAX; i++) {
        string path = getImagePath((IMAGE_NAME)i);   // Obté la ruta d'imatge
        m_aSprites[i].create(path.c_str());          // Crea el sprite amb la imatge corresponent
    }
}

// Dibuixa un sprite a la posició (posX, posY)
void GraphicManager::drawSprite(IMAGE_NAME name, float posX, float posY)
{
    if (name >= 0 && name < GRAFIC_NUM_MAX)
        m_aSprites[name].draw(posX, posY); // Dibuixa el sprite corresponent
    else
        cout << "Error: no existeix el identificador de image"; // Error si no és vàlid
}

// Dibuixa text a la pantalla amb la font especificada i retorna la seva àrea
NFont::Rectf GraphicManager::drawFont(FONT_NAME name, float posX, float posY, float size, const string& msg)
{
    switch (name) {
    case FONT_GREEN_30:
        return m_oFontGreen_30.draw(posX, posY, NFont::Scale(size), "%s", msg.c_str());
    case FONT_RED_30:
        return m_oFontRed_30.draw(posX, posY, NFont::Scale(size), "%s", msg.c_str());
    case FONT_WHITE_30:
        return m_oFontWhite_30.draw(posX, posY, NFont::Scale(size), "%s", msg.c_str());
    default:
        cout << "Error: no existeix el identificador de font";
        break;
    }
    return NFont::Rectf(); // Retorna un rectangle buit per defecte en cas d'error
}

// Retorna la ruta d'imatge segons el tipus de gràfic
string GraphicManager::getImagePath(IMAGE_NAME name) {
    string path;
    switch (name) {
    case GRAFIC_FONS:
        path = "data/GUI/fons.png";
        break;
    case GRAFIC_TAULER:
        path = "data/GUI/tauler2.1.png";
        break;
    case GRAFIC_POSICIO_VALIDA:
        path = "data/GUI/posicio_valida.png";
        break;
    case GRAFIC_FITXA_BLANCA:
        path = "data/Fitxes/fitxa_blanca.png";
        break;
    case GRAFIC_DAMA_BLANCA:
        path = "data/Fitxes/dama_blanca.png";
        break;
    case GRAFIC_FITXA_NEGRA:
        path = "data/Fitxes/fitxa_negra.png";
        break;
    case GRAFIC_DAMA_NEGRA:
        path = "data/Fitxes/dama_negra.png";
        break;
    default:
        path = ""; // Retorna cadena buida si el nom no és reconegut
        break;
    }
    return path;
}

// Retorna la ruta del fitxer de font segons el nom
string GraphicManager::getFontPath(FONT_NAME name) {
    string path;
    switch (name) {
    case FONT_GREEN_30:
    case FONT_RED_30:
    case FONT_WHITE_30:
        path = "data/Fonts/FreeSans.ttf"; // Totes les fonts apunten al mateix arxiu
        break;
    default:
        path = ""; // Valor per defecte si no és reconegut
        break;
    }
    return path;
}
