#ifndef __MODULO_F__
#define __MODULO_F__

#include "funcoesAuxiliares.h"

unsigned long long int calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada);

int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco);

void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado);

#endif //MODULO_F_MODULOF_H
