#ifndef __MODULO_F__
#define __MODULO_F__

#include "../../shafa.h"

#define K 655360
#define m 8388608
#define M 67108864
#define r 1

#include "funcoesAuxiliares.h"

void moduleFMain(Options *opts);

unsigned long long int calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada);

int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco);

void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado);

#endif //MODULO_F_MODULOF_H
