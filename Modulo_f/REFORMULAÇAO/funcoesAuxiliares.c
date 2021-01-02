#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoesAuxiliares.h"
#include "fsize.h"


char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao) {
    size_t length = strlen(nomeFicheiro) + strlen(extensao) + 1;
    char *concat = malloc(sizeof(char) * length);
    if (!concat) return NULL; // Malloc error
    snprintf(concat, length, "%s%s", nomeFicheiro, extensao);

    return concat;
}

FicheiroInf NBlocos(FILE *f, char *nomeFicheiro, unsigned long int tamanhoBloco) { // TamanhoBloco vem em Bytes!!!
    FicheiroInf fic = malloc(sizeof(struct ficheiroInf));
    fic -> tamanhoBloco = tamanhoBloco;
    fic -> nomeFicheiro = nomeFicheiro;
    fic -> numBloco = (unsigned long long int) fsize(f, NULL, &(fic -> tamanhoBloco), &(fic -> tamanhoUltimoBloco));
    fic -> tamanhoTotal = (fic -> numBloco - 1) * (fic -> tamanhoBloco) + (fic -> tamanhoUltimoBloco);
    fic -> ficheiros =  malloc(sizeof(struct Ficheiros));
    return fic;
}

unsigned long int tamanhoBloco(FicheiroInf fInf, unsigned long long numBloco) {
    return (numBloco != fInf -> numBloco - 1) ? fInf -> tamanhoBloco : fInf -> tamanhoUltimoBloco;
}

Byte *leituraFicheiro(FILE *f, unsigned long int tamanhoBloco) {
    //static Byte buffer[1];
    fseek(f, 0L,SEEK_SET);
    Byte *buffer = malloc(sizeof(Byte)*tamanhoBloco);
    int i = fread(buffer, sizeof(Byte), tamanhoBloco, f);
    int er = ferror(f);
    int eop = feof(f);
    printf("%d Bytes lidos.\n", i);
    return buffer;
}

void escritaFicheiro(FILE *f, Byte *buffer, unsigned long int tamanho) {
    fwrite(buffer, sizeof(Byte), tamanho, f);
}

void libertarEspacoInfosBloco (InfosBloco infosBloco) {
    if (infosBloco -> BufferSimbolos) free(infosBloco -> BufferFreqs);
    free(infosBloco -> BufferFreqs);
    free(infosBloco);
}

void data() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%02d-%02d-%d \n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}