#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/**
 * @file moduloF.c
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções principais do nosso módulo.
 *
 * A funções deste ficheiro usam, principalmente, funções do
 * processamento para gerar os ficheiros necessários do
 * nosso módulo, bem como para imprimir no terminal a
 * informação pertinente.
 *
 */

#include "moduloF.h"
#include "funcoesAuxiliares.h"
#include "processamento.h"
#include "../shafa.h"


void moduleFMain(Options *opts) {
    moduloF(opts -> fileIN, opts -> optC, opts -> optB);
}

int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco) {
    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Calculo do número de blocos
    FicheiroInf fInf = NBlocos(nomeFicheiro, tamanhoBloco);
    if (!fInf) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }
    // Abertura do ficheiro
    FILE *orig = fopen(nomeFicheiro, "rb"); // Ficheiro original

    unsigned long long tamanhoRle = calculoFrequencias(orig, fInf, compressaoForcada);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Informações a aparecer na consola:
    infoTerminal(fInf, tamanhoRle, inicio, fim);

    // Free fInf
    freeFicheiroInf(fInf);

    fclose(orig);
    return 0;
}


unsigned long long int calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada) {
    unsigned long long numBloco;
    unsigned long long tamanhoRLE = 0;
    unsigned long long *tamanhoRLEPointer = &tamanhoRLE;

    for(numBloco = 0; numBloco < fInf -> numBloco; numBloco++) {
        calculoFrequenciasBloco(orig, fInf, numBloco, compressaoForcada, &tamanhoRLEPointer);
    }
    return tamanhoRLE;
}


void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado) {
    // Leitura do bufferInput
    Byte *bufferInput = leituraFicheiro(orig, tamanhoBloco(fInf, numBloco));
    InfosBloco infosBloco;

    // Calculamos as frequências do rle ou do original?
    // Calculamos a compressão rle do primeiro bloco
    // Se tamanhoBlocoRle / tamanhoBlocoOriginal < 0.95, efetuamos a compressao rle

    // Primeiro bloco
    if (!numBloco) {
        infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);
        if (compressaoForcada) {
            char *rle = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle");
            char *rlefreq = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq");
            (fInf -> ficheiros) -> rle = fopen(rle, "wb");
            (fInf -> ficheiros) -> rleFreqs = fopen(rlefreq, "w");
            free(rle);
            free(rlefreq);
        } else {
            double TaxaCompressao = (double)**tamanhoRleAcumulado / (double)tamanhoBloco(fInf, numBloco);
            if (TaxaCompressao > 0.95) { // Nao fazemos RLE
                char *freq = nomeFicheiroExtensao(fInf->nomeFicheiro, ".freq");
                (fInf->ficheiros)->origFreqs = fopen(freq, "w");
                free(freq);
                *tamanhoRleAcumulado = NULL;
                infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);
            } else {
                char *rle = nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle");
                char *rlefreq = nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle.freq");
                (fInf->ficheiros)->rle = fopen(rle, "wb");
                (fInf->ficheiros)->rleFreqs = fopen(rlefreq, "w");
                free(rle);
                free(rlefreq);
            }
        }
    }
    else infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);

    // Escrita no ficheiro de dados do bufferOutput
    if (!*tamanhoRleAcumulado) escritaFicheiro((fInf -> ficheiros) -> origFreqs, infosBloco -> BufferFreqs, infosBloco -> tamanhoBufferFreqs);
    else {
        escritaFicheiro((fInf -> ficheiros) -> rle, infosBloco -> BufferSimbolos, infosBloco -> tamanhoBufferRle);
        escritaFicheiro((fInf -> ficheiros) -> rleFreqs, infosBloco -> BufferFreqs, infosBloco -> tamanhoBufferFreqs);
    }


    // Fechar os ficheiros
    if (numBloco == fInf -> numBloco - 1) {
        if (!*tamanhoRleAcumulado) fclose((fInf -> ficheiros) -> origFreqs);
        else {
            fclose((fInf -> ficheiros) -> rle);
            fclose((fInf -> ficheiros) -> rleFreqs);
        }
        free(fInf -> ficheiros);
    }
    // Libertar espaço dos buffer
    free(bufferInput);
    libertarEspacoInfosBloco(infosBloco);
}
