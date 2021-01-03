#ifndef __THREADING__
#define __THREADING__

#include "funcoesAuxiliares.h"


int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco);
void calculoFrequenciasBloco1(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado);
void* calculoFrequenciasBlocoTHREAD(void* argsThread);
unsigned long long int calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada);



#endif 
