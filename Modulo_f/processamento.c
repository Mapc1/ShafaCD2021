/**
 * @file processamento.c
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções necessárias para processar o ficheiro de entrada .
 *
 * A funções deste ficheiro processam o buffer(array de "Bytes", que são
 * unsigned chars) onde foi guardado o ficheiro de entrada.
 * O objetivo passou por criar outros dois buffers, um onde foi guardada
 * a informação que será colocada no ficheiro das frequências e outro
 * onde será guardada a informação que será colocada no ficheiro RLE
 * (se este for gerado).
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoesAuxiliares.h"
#include "processamento.h"

void freqsOriginal(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs) {
    unsigned long long int i;
    // Contagem de símbolos
    for (i = 0; i < tamanhoBlocoInput; i++) BufferFreqs[bufferInput[i]] ++;
}

void freqsRle(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs, InfosBloco infosBloco, unsigned long long *tamanhoRleAcumulado) {
    Byte *BufferSimbolos = malloc(sizeof (Byte) * tamanhoBlocoInput);
    unsigned long long espacoAlocado = 8;
    unsigned long long int i = 0;
    infosBloco -> tamanhoBufferRle = 0;
    while (i < tamanhoBlocoInput) { // Analisar um símbolo de cada vez
        if (infosBloco -> tamanhoBufferRle > espacoAlocado - 4) {
            espacoAlocado *= 2;
            BufferSimbolos = (void *) realloc(BufferSimbolos, espacoAlocado*sizeof(Byte));
        }
        Byte num_repeticoes = 1;
        Byte simbolo = bufferInput[i];
        // Identificação de repetições;
        for (; i < tamanhoBlocoInput; i++) {
            if (i == tamanhoBlocoInput - 1 || (simbolo != bufferInput[i + 1] || num_repeticoes == 255)) {
                i++;
                break;
            } else num_repeticoes++;
        }
        // Gerar BUFFER temporario com a compressao RLE
        if (num_repeticoes >= 4 ||
            simbolo == 0) { // O símbolo repete-se pelo menos 4 vezes ou o símbolo ser 0, logo aplicamos o padrão RLE
            Byte padraoRLE[3] = {0, simbolo, num_repeticoes};// {0}{simbolo}{número_de_repetições}
            int k = 0;
            unsigned long long j = infosBloco -> tamanhoBufferRle;
            for (; infosBloco -> tamanhoBufferRle < j + 3; infosBloco -> tamanhoBufferRle++) BufferSimbolos[infosBloco -> tamanhoBufferRle] = padraoRLE[k++];
            // Contagem das frequências dos símbolos do ficheiro RLE
            BufferFreqs[0]++;
            BufferFreqs[simbolo]++;
            BufferFreqs[num_repeticoes]++;
        } else { // O símbolo não se repete pelo menos 4 vezes, logo não aplicamos o padrão RLE
            // Contagem do número de símbolos do ficheiro
            unsigned long long j = infosBloco -> tamanhoBufferRle;
            for (; infosBloco -> tamanhoBufferRle < j + num_repeticoes; infosBloco -> tamanhoBufferRle++) {
                BufferSimbolos[infosBloco -> tamanhoBufferRle] = simbolo;
            }
            BufferFreqs[simbolo] += (unsigned long long)num_repeticoes;
        }
    }
    infosBloco -> BufferSimbolos = BufferSimbolos;
    *tamanhoRleAcumulado += infosBloco -> tamanhoBufferRle;
}

void freqsParaEscrita(unsigned long long *BufferFreqs, unsigned long long numBloco, FicheiroInf fInf, InfosBloco infosBloco) {
    unsigned long long espacoAlocado = 396800; //256*1550 //?
    infosBloco -> BufferFreqs = malloc(sizeof (Byte) * espacoAlocado);
    infosBloco -> tamanhoBufferFreqs = 0;
    unsigned int i;
    char *local = (char*) &(infosBloco -> BufferFreqs[infosBloco -> tamanhoBufferFreqs]);

    if (!numBloco) infosBloco -> tamanhoBufferFreqs += sprintf(local, "@%c@%lld", !(infosBloco -> BufferSimbolos) ? 'N': 'R', fInf -> numBloco);
    local = (char*) &(infosBloco -> BufferFreqs[infosBloco -> tamanhoBufferFreqs]);
    infosBloco -> tamanhoBufferFreqs += sprintf(local, "@%llu@", !(infosBloco -> BufferSimbolos) ? tamanhoBloco(fInf, numBloco): infosBloco -> tamanhoBufferRle);
    for (i = 0; i < 256; i++) { // Otimizar condições!!!!!
        local = (char*) &(infosBloco -> BufferFreqs[infosBloco -> tamanhoBufferFreqs]);
        if (i == 255 && (BufferFreqs[i] == BufferFreqs[i - 1]));
	else if (i == 255 && (BufferFreqs[i] != BufferFreqs[i - 1])) infosBloco->tamanhoBufferFreqs += sprintf(local, "%llu", BufferFreqs[i]);
        else if (!i || ((BufferFreqs[i] != BufferFreqs[i - 1]))) {
            infosBloco->tamanhoBufferFreqs += sprintf(local, "%lld;", (BufferFreqs[i]));
        }
        else if (BufferFreqs[i] == BufferFreqs[i - 1]) infosBloco->tamanhoBufferFreqs += sprintf(local, ";");
        else if (i != 255) infosBloco->tamanhoBufferFreqs += sprintf(local, "%lld", (BufferFreqs[i]));
        else
            infosBloco->tamanhoBufferFreqs += sprintf(local, "%lld;", (BufferFreqs[i])); // (aux_Freqs->FicheiroOriginal[i] != aux_Freqs->FicheiroOriginal[i-1])
        if (infosBloco->tamanhoBufferRle > espacoAlocado - 20) {
            espacoAlocado *= 2;
            infosBloco->BufferFreqs = realloc(infosBloco->BufferFreqs, espacoAlocado);
        }
        
    }
    if (numBloco == fInf -> numBloco - 1) {
        local = (char*) &(infosBloco -> BufferFreqs[infosBloco -> tamanhoBufferFreqs]);
        infosBloco->tamanhoBufferFreqs += sprintf(local, "@0");
    }
}



InfosBloco processamento(Byte *bufferInput, FicheiroInf fInf, unsigned long long numBloco, unsigned long long *tamanhoRleAcumulado){
    // Tamanho do bloco
    unsigned long int tamanhoBlocoInput = tamanhoBloco(fInf, numBloco);
   
    //struct com as informações sobre o bloco
    InfosBloco infosBloco = malloc(sizeof (struct infosBloco));
    unsigned long long int *BufferFreqs = malloc(sizeof (unsigned long long int) * 256);
    
    // Inicializar o buffer das frequencias a 0
    int i;
    for(i = 0; i < 256; i++) BufferFreqs[i] = 0;

    if (!tamanhoRleAcumulado) { // RLE = NÃO;
        // Contar frequencias do Original
        freqsOriginal(bufferInput, tamanhoBlocoInput, BufferFreqs);
        infosBloco -> BufferSimbolos = NULL;
        infosBloco -> tamanhoBufferRle = 0;
    } else { // RLE = SIM;
        freqsRle(bufferInput, tamanhoBlocoInput, BufferFreqs, infosBloco, tamanhoRleAcumulado);
    }
    // Transformar o array com as frequências num Byte * para a escrita no ficheiro freq
    freqsParaEscrita(BufferFreqs, numBloco, fInf, infosBloco);

    free(BufferFreqs);

    return infosBloco;
}
