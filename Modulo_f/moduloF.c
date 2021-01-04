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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "moduloF.h"
#include "funcoesAuxiliares.h"
#include "processamento.h"
#include "../shafa.h"


void moduleFMain(Options *opts){
    int tamanhoBloco;

    int compressaoForcada; 

    if(opts->optC == 'r') compressaoForcada = 1;
    else compressaoForcada = 0;

    switch(opts->optB){
        case 'K': tamanhoBloco = K; break;
        case 'm': tamanhoBloco = m; break;
        case 'M': tamanhoBloco = M; break;
        default : tamanhoBloco = 65536;  
    }
	
    moduloF(opts -> fileIN, opts -> fileOUT, compressaoForcada, tamanhoBloco);   
    
}

int moduloF(char *nomeFicheiro, char *nomeFicheiroSaida, char compressaoForcada, unsigned long tamanhoBloco) {
    printf("A processar...\n");
    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Calculo do número de blocos
    FicheiroInf fInf = NBlocos(nomeFicheiro, nomeFicheiroSaida, tamanhoBloco);
    if (!fInf) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }
    // Abertura do ficheiro
    FILE *orig = fopen(nomeFicheiro, "rb"); // Ficheiro original

    unsigned long long *tamanhoRlePointer = calculoFrequencias(orig, fInf, compressaoForcada);
 
    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Informações a aparecer na consola:
    infoTerminal(fInf, tamanhoRlePointer, inicio, fim);

    // Free fInf
    freeFicheiroInf(fInf);

    fclose(orig);
    return 0;
}


unsigned long long int *calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada) {
    unsigned long long numBloco;
    static unsigned long long tamanhoRLE = 0;
    unsigned long long *tamanhoRLEPointer = &tamanhoRLE;

    for(numBloco = 0; numBloco < fInf -> numBloco; numBloco++) calculoFrequenciasBloco(orig, fInf, numBloco, compressaoForcada, &tamanhoRLEPointer);
    
    return tamanhoRLEPointer; 
}


void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado) {
    // Leitura do bufferInput
    Byte *bufferInput = leituraFicheiro(orig, tamanhoBloco(fInf, numBloco));
    InfosBloco infosBloco;

    // Calculamos as frequências do rle ou do original?
    // Calculamos a compressão rle do primeiro bloco
    // Se tamanhoBlocoRle / tamanhoBlocoOriginal < 0.95, efetuamos a compressao rle
    //
    // Primeiro bloco
    if (!numBloco) {
        infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);
        if (compressaoForcada) {
            char *rle = nomeFicheiroExtensao(fInf -> nomeFicheiroSaida, ".rle");
            char *rlefreq = nomeFicheiroExtensao(fInf -> nomeFicheiroSaida, ".rle.freq");
            (fInf -> ficheiros) -> rle = fopen(rle, "wb");
            (fInf -> ficheiros) -> rleFreqs = fopen(rlefreq, "w");
            free(rle);
            free(rlefreq);
        } else {
            double TaxaCompressao = (double)**tamanhoRleAcumulado / (double)tamanhoBloco(fInf, numBloco);
            if (TaxaCompressao > 0.95) { // Nao fazemos RLE
                char *freq = nomeFicheiroExtensao(fInf->nomeFicheiroSaida, ".freq");
                (fInf->ficheiros)->origFreqs = fopen(freq, "w");
                free(freq);
                *tamanhoRleAcumulado = NULL;
                infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);
            } else {
                char *rle = nomeFicheiroExtensao(fInf->nomeFicheiroSaida, ".rle");
                char *rlefreq = nomeFicheiroExtensao(fInf->nomeFicheiroSaida, ".rle.freq");
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
