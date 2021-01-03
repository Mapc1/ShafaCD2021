#ifndef __MODULO_F__
#define __MODULO_F__

#include "funcoesAuxiliares.h"


int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco);

void* calculoFrequenciasBlocoTHREAD(void* argsThread);

void calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada);

#endif //MODULO_F_MODULOF_H
