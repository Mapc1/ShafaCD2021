#ifndef MODULO_F_MODULO_F_H
#define MODULO_F_MODULO_F_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define TAMANHO_BLOCO 512
#define TAMANHO_MINIMO_ULTIMO_BLOCO 1024

typedef struct ficheiroInf {
    unsigned long long int tamanhoTotal;
    unsigned long long int tamanhoBloco;
    unsigned long long int tamanhoUltimoBloco;
    unsigned long long int num_blocos;
} *FicheiroInf;

typedef struct freqsInf { // Struct usada na função compressaoRLEBloco para ao comprimir o ficheiro original para rle, contar as frequências do símbolos do ficheiro original e do ficheiro rle
    unsigned long long int *FicheiroOriginal;
    unsigned long long int *FicheiroRLE;
} *FreqsInf;

FicheiroInf NBlocos(FILE *f, unsigned long long int tamanhoBloco, unsigned long long int tamanhoMinimoUltimoBloco);

unsigned long long int tamanhoFicheiro (FILE *f);

char *Bloco_to_array(FILE *f, FicheiroInf fInf, unsigned long long int num_bloco);

double compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, char compressaoForcada);

FreqsInf compressaoRLEBloco(FILE *orig, FicheiroInf fInf, FILE *rle, unsigned long long int num_bloco);

void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freqOrig, FILE *freqRLE, unsigned long long int numBloco);

#endif //MODULO_F_MODULO_F_H
