#ifndef MODULO_F_MODULO_F_H
#define MODULO_F_MODULO_F_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define TAMANHO_BLOCO 512
#define TAMANHO_MINIMO_ULTIMO_BLOCO 1024


typedef struct ficheiroInf {
    size_t tamanhoTotal;
    size_t tamanhoBloco;
    size_t tamanhoUltimoBloco;
    int num_blocos;
} *FicheiroInf;

FicheiroInf NBlocos(FILE *f, size_t tamanhoBloco, size_t tamanhoMinimoUltimoBloco);

size_t tamanhoFicheiro (FILE *f);

char *Bloco_to_array(FILE *f, FicheiroInf fInf, int num_bloco);

size_t tamMax_Array (size_t tamanhoBloco, size_t tamanhoUltimoBloco);

double compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, char compressaoForcada);

void compressaoRLEBloco(FILE *orig, FicheiroInf fInf, FILE *rle, size_t num_bloco);

void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freq, char tipoFicheiro, int numBloco);

void frequencias(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freq, char tipoFicheiro);

#endif //MODULO_F_MODULO_F_H
