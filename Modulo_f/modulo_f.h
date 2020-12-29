#ifndef MODULO_F_MODULO_F_H
#define MODULO_F_MODULO_F_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fsize.h"

#define TAMANHO_BLOCO 512
#define TAMANHO_MINIMO_ULTIMO_BLOCO 1024
#define RLE_SIM 1
#define RLE_NAO 0

typedef unsigned char Byte;

typedef struct ficheiroInf {
    char *nomeFicheiro;
    unsigned long long int tamanhoTotal;
    unsigned long int tamanhoBloco;
    long int tamanhoUltimoBloco;
    long long int numBloco;
} *FicheiroInf;


typedef struct ficheiro_RLE_Inf {
    unsigned long long int tamanhoBlocoRleAcumulado;
    unsigned long long int tamanhoUltimoBlocoRle;
} *FicheiroRleInf;

typedef struct freqsInf { // Struct usada na função compressaoRLEBloco para ao comprimir o ficheiro original para rle, contar as frequências do símbolos do ficheiro original e do ficheiro rle
    unsigned long long int *FicheiroOriginal;
    unsigned long long int *FicheiroRLE;
} *FreqsInf;

void data();

char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao);

FicheiroInf NBlocos(FILE *f, char *nomeFicheiro, unsigned long int tamanhoBloco);

FicheiroRleInf NBlocosRle(double TaxaCompressao, FicheiroInf fInf, FILE *rle);

char *novoficheiro(char *tipoficheiro, FicheiroInf fInf);

unsigned long long int tamanhoFicheiro (FILE *f);

unsigned char *Bloco_to_array(FILE *f, FicheiroInf fInf, unsigned long long int numBloco);

void
escrita_freqs(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, FicheiroRleInf RleInf, int compr);

FicheiroRleInf compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, char compressaoForcada);

FreqsInf
compressaoRLEBloco(FILE *orig, FicheiroInf fInf, FILE *rle, FicheiroRleInf RleInf, unsigned long long int numBloco);

void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freqOrig, FILE *freqRLE,
                       FicheiroRleInf RleInf, unsigned long long int numBloco);

void ficheiros_gerados(FicheiroInf fInf, FicheiroRleInf RleInf);



#endif //MODULO_F_MODULO_F_H
