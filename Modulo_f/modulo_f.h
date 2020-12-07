#ifndef MODULO_F_MODULO_F_H
#define MODULO_F_MODULO_F_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define NumBytesLidos 1


typedef struct ficheiroInf {
    size_t tamanhoTotal;
    size_t tamanhoBloco;
    size_t tamanhoUltimoBloco;
    int num_blocos;
} *FicheiroInf;

FicheiroInf NBlocos(FILE *f, size_t tamanhoBloco, size_t tamanhoMinimoUltimoBloco);
size_t tamanhoFicheiro (FILE *f);
void Bloco1_to_array(FILE *f, FicheiroInf fInf);
size_t tamMax_Array (size_t tamanhoBloco, size_t tamanhoUltimoBloco);

#endif //MODULO_F_MODULO_F_H
