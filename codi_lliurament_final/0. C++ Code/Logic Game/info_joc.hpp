#ifndef INFO_JOC_H
#define INFO_JOC_H

// Posició del tauler a la pantalla
const int POS_X_TAULER = 200;
const int POS_Y_TAULER = 0;

// Posició de la primera casella del tauler respecte a la cantonada superior esquerra del gràfic del tauler
const int CASELLA_INICIAL_X = 63;
const int CASELLA_INICIAL_Y = 186;


// Tamany de les caselles del tauler
const int AMPLADA_CASELLA = 62;
const int ALCADA_CASELLA = 62;

const int NUM_COLS_TAULER = 8;
const int NUM_FILES_TAULER = 8;

const int TAMANY_PANTALLA_X = 900;
const int TAMANY_PANTALLA_Y = 900;

typedef enum 
{
    MODE_JOC_NORMAL,
    MODE_JOC_REPLAY,
    MODE_JOC_NONE
} ModeJoc;

#endif 
