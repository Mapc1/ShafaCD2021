#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "moduloF.h"
#include "funcoesAuxiliares.h"
#include "processamento.h"


int main() {
    char nomeFicheiro[30] = "Teste/teste/Shakespeare.txt"; // Argv[1] // Aumentado devido a alguns nomes dos testes
    char compressaoForcada = 1;  // 1 se quisermos forçar compressão, senão 0
    unsigned long tamanhoBloco = 655360;
    moduloF(nomeFicheiro, compressaoForcada, tamanhoBloco);
    return 0;
}

int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco) {
    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Calculo do número de blocos
    FicheiroInf fInf = NBlocos(nomeFicheiro, tamanhoBloco);

    // Abertura dos ficheiros
    FILE *orig;
    orig = fopen(nomeFicheiro, "r"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }

    unsigned long long tamanhoRle = calculoFrequencias(orig, fInf, compressaoForcada);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Informações a aparecer na consola:
    infoTerminal(fInf, tamanhoRle, inicio, fim);

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
            (fInf -> ficheiros) -> rle = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle"), "wb");
            (fInf -> ficheiros) -> rleFreqs = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq"), "w");
        } else {
            double TaxaCompressao = (double)**tamanhoRleAcumulado / (double)tamanhoBloco(fInf, numBloco);
            if (TaxaCompressao > 0.95) { // Nao fazemos RLE
                (fInf->ficheiros)->origFreqs = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".freq"), "w");
                *tamanhoRleAcumulado = NULL;
                infosBloco = processamento(bufferInput, fInf, numBloco, *tamanhoRleAcumulado);
            } else {
                (fInf->ficheiros)->rle = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle"), "wb");
                (fInf->ficheiros)->rleFreqs = fopen(nomeFicheiroExtensao(fInf->nomeFicheiro, ".rle.freq"), "w");
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
    }
    // Libertar espaço dos buffer
    free(bufferInput);
    libertarEspacoInfosBloco(infosBloco);
}
