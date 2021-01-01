#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moduloF.h"
#include "funcoesAuxiliares.h"
#include "processamento.h"


int main() {
    char nomeFicheiro[30] = "Teste/aaaa/aaaa.txt"; // Argv[1] // Aumentado devido a alguns nomes dos testes
    char compressaoForcada = 0;  // 1 se quisermos forçar compressão, senão 0
    //unsigned long tamanhoBloco = 0;
    moduloF(nomeFicheiro, compressaoForcada);


    return 0;
}

int moduloF(char *nomeFicheiro, char compressaoForcada) {
    // Abertura dos ficheiros
    FILE *orig;
    orig = fopen(nomeFicheiro, "r"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }
    unsigned long tamanhoBloco = 2048;
    // Calculo do número de blocos
    FicheiroInf fInf = NBlocos(orig, nomeFicheiro, tamanhoBloco);
    //data();


    calculoFrequencias(orig, fInf, compressaoForcada);

    fclose(orig);
    return 0;
}


void calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada) {
    unsigned long long numBloco;
    unsigned long long tamanhoRLE = 0;
    for(numBloco = 0; numBloco < fInf -> numBloco; numBloco++) {
        calculoFrequenciasBloco(orig, fInf, numBloco, compressaoForcada, &tamanhoRLE);
    }
}


void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long *tamanhoRleAcumulado) {
    // Leitura do bufferInput
    Byte *bufferInput = leituraFicheiro(orig, tamanhoBloco(fInf, numBloco));
    InfosBloco infosBloco;

    // Calculamos as frequências do rle ou do original?
    // Calculamos a compressão rle do primeiro bloco
    // Se tamanhoBlocoRle / tamanhoBlocoOriginal < 0.95, efetuamos a compressao rle

    // Primeiro bloco
    if (!numBloco) {
        infosBloco = processamento(bufferInput, fInf, numBloco, tamanhoRleAcumulado);

        if (compressaoForcada) {
            (fInf -> ficheiros) -> rle = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle"), "wb");
            (fInf -> ficheiros) -> rleFreqs = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq"), "w");
        } else {
            double TaxaCompressao = (double)*tamanhoRleAcumulado / (double)tamanhoBloco(fInf, numBloco);
            if (TaxaCompressao > 0.95) { // Nao fazemos RLE
                (fInf->ficheiros)->origFreqs = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".freq"), "w");
                tamanhoRleAcumulado = NULL;
            } else {
                (fInf->ficheiros)->rle = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle"), "wb");
                (fInf->ficheiros)->rleFreqs = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle.freq"), "w");
            }
        }
    }
    else infosBloco = processamento(bufferInput, fInf, numBloco, tamanhoRleAcumulado);

    // Escrita no ficheiro de dados do bufferOutput
    if (!tamanhoRleAcumulado) escritaFicheiro((fInf -> ficheiros) -> origFreqs, infosBloco -> BufferSimbolos, infosBloco -> tamanhoBufferFreqs);
    else {
        escritaFicheiro((fInf -> ficheiros) -> rle, infosBloco -> BufferSimbolos, infosBloco -> tamanhoBufferRle);
        escritaFicheiro((fInf -> ficheiros) -> rleFreqs, infosBloco -> BufferFreqs, infosBloco -> tamanhoBufferFreqs);
    }


    // Fechar os ficheiros
    if (numBloco == fInf -> numBloco - 1) {
        if (!tamanhoRleAcumulado) fclose((fInf -> ficheiros) -> origFreqs);
        else {
            fclose((fInf -> ficheiros) -> rle);
            fclose((fInf -> ficheiros) -> rleFreqs);
        }
    }
    // Libertar espaço dos buffer
    free(bufferInput);
    //libertarEspacoInfosBloco(infosBloco);
}
