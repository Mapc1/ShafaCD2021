/**
 * @file funcoesAuxiliares.c
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções auxiliares.
 *
 * A funções deste ficheiro são funções que não interferem com o
 * processamento da informação dos ficheiros, sendo funções
 * que ajudam na correta execução do programa.
 * Neste grupo de funções, temos, por exemplo, funções que gerem a 
 * forma como a informação é apresentada no terminal.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoesAuxiliares.h"
#include "fsize.h"
#include "../shafa.h"


char *nomeFicheiroExtensao(const char *nomeFicheiro, const char *extensao) {
    size_t length = strlen(nomeFicheiro) + strlen(extensao) + 1;
    char *concat = malloc(sizeof(char) * length);
    if (!concat) return NULL; // Malloc error
    snprintf(concat, length, "%s%s", nomeFicheiro, extensao);

    return concat;
}

FicheiroInf NBlocos(char *nomeFicheiro, unsigned long int tamanhoBloco) { // TamanhoBloco vem em Bytes!!!
    FicheiroInf fInf = malloc(sizeof(struct ficheiroInf));
    fInf -> tamanhoBloco = tamanhoBloco;
    fInf -> nomeFicheiro = nomeFicheiro;
    long long returnFSize = fsize(NULL, (unsigned char*)nomeFicheiro, &(fInf -> tamanhoBloco), &(fInf -> tamanhoUltimoBloco));
    if (returnFSize < 0) { // Erro na fsize
        free(fInf);
        return NULL;
    }
    fInf -> numBloco = (unsigned long long int) returnFSize;
    fInf -> ficheiros =  malloc(sizeof(struct Ficheiros));
    fInf -> tamanhoTotal = (fInf -> numBloco - 1) * (fInf -> tamanhoBloco) + (fInf -> tamanhoUltimoBloco);
    return fInf;
}

unsigned long int tamanhoBloco(FicheiroInf fInf, unsigned long long numBloco) {
    return (numBloco != fInf -> numBloco - 1) ? fInf -> tamanhoBloco : fInf -> tamanhoUltimoBloco;
}

Byte *leituraFicheiro(FILE *f, unsigned long int tamanhoBloco) {
    Byte *buffer = malloc(tamanhoBloco*sizeof(Byte));
    fread(buffer, sizeof(Byte), tamanhoBloco, f);
    return buffer;
}

void escritaFicheiro(FILE *f, Byte *buffer, unsigned long int tamanho) {
    fwrite(buffer, sizeof(Byte), tamanho, f);
}

void libertarEspacoInfosBloco (InfosBloco infosBloco) {
    if (infosBloco -> BufferSimbolos) free(infosBloco -> BufferSimbolos);
    free(infosBloco -> BufferFreqs);
    free(infosBloco);
}

void freeFicheiroInf(FicheiroInf fInf) {
    free(fInf);
}



void ficheiros_gerados(FicheiroInf fInf, Byte RleEfetuado) {
    char *freqs_Original = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".freq");
    char *RLE = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle");
    char *freqs_RLE = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq");
    if (RleEfetuado) printf("%s, %s\n", RLE, freqs_RLE);
    else printf("%s\n", freqs_Original);
    free(freqs_Original);
    free(RLE);
    free(freqs_RLE);
}

void infoTerminal(FicheiroInf fInf, unsigned long long *tamanhoRle, clock_t inicio, clock_t fim) {
    printf("\n%p\n", tamanhoRle);
    printf("\n%lld\n", *tamanhoRle);
    printf("Miguel Martins, a93280, Gonçalo Soares, a93286, MIEI/CD, ");
    data();
    printf("Módulo: f (Cálculo das frequências dos símbolos)\n");
    printf("Número de blocos: %llu  \n", fInf -> numBloco);
    if (fInf -> numBloco > 1) printf("Tamanho dos blocos analisados no ficheiro original: %lu/%lu\n", fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco);
    else printf("Tamanho do bloco analisado no ficheiro original: %lu\n", fInf -> tamanhoUltimoBloco);
    Byte rleEfetuado;
    if (!tamanhoRle) {
        printf("Compressão RLE: Não efetuada\n");
	rleEfetuado = 0;
    } else {
        double TaxaCompressao = (double) (*tamanhoRle) / (double) fInf -> tamanhoTotal;
        printf("Compressão RLE: %s.rle (%lf%% compressão)\n", fInf->nomeFicheiro, TaxaCompressao); 
        printf("Tamanho do ficheiro RLE: %llu\n", *tamanhoRle); 
	rleEfetuado = 1;
    }
    printf("Tempo de execução do módulo: %f milisegundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000);
    printf("Ficheiros gerados: ");    
    printf("%u\n", rleEfetuado);
    ficheiros_gerados(fInf, rleEfetuado);
}
