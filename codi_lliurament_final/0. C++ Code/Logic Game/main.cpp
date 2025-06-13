//
//  main.cpp
//
//  Copyright � 2018 Compiled Creations Limited. All rights reserved.
//

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined  (_WIN64)

#include <iostream>
//Definicio necesaria per poder incloure la llibreria i que trobi el main
#define SDL_MAIN_HANDLED
#include <windows.h>
//Llibreria grafica
#include "../Graphic Lib/libreria.h"
#include "../Graphic Lib/NFont/NFont.h"
#include <conio.h>      /* getch */ 

#elif __APPLE__

#endif

#include "./joc.hpp"
#include "./info_joc.hpp"


int main(int argc, const char* argv[])
{
    //Instruccions necesaries per poder incloure la llibreria i que trobi el main
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    //Inicialitza un objecte de la classe Screen que s'utilitza per gestionar la finestra grafica
    Screen pantalla(TAMANY_PANTALLA_X, TAMANY_PANTALLA_Y);
    //Mostrem la finestra grafica
    pantalla.show();

    Joc joc;
    joc.inicialitza(MODE_JOC_REPLAY, "tauler_inicial.txt", "moviments.txt");

    do
    {
        // Captura tots els events de ratolí i teclat de l'ultim cicle
        pantalla.processEvents();

        bool mouseStatus = Mouse_getBtnLeft();
        static bool mouseWasPressed = false;

        int mousePosX = Mouse_getX();
        int mousePosY = Mouse_getY();

        // Només executa si el clic COMENÇA (baixada)
        bool clicNou = mouseStatus && !mouseWasPressed;

        bool final = joc.actualitza(mousePosX, mousePosY, clicNou);

        // Actualitza l'estat del botó per la següent iteració
        mouseWasPressed = mouseStatus;


        // Actualitza la pantalla
        pantalla.update();

    } while (!Keyboard_GetKeyTrg(KEYBOARD_ESCAPE));
    // Sortim del bucle si pressionem ESC
    joc.finalitza();

    //Instruccio necesaria per alliberar els recursos de la llibreria 
    SDL_Quit();
    return 0;
}

