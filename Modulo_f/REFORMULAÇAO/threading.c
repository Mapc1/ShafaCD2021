#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "funcoesAuxiliares.h"
#include "fsize.h"
#include "threading.h"
#include "processamento.h"
#include "moduloF.h"



//main do nosso programa
/*

int main() {
    char nomeFicheiro[30] = "Teste/aaaa/aaaa.txt"; // Argv[1] // Aumentado devido a alguns nomes dos testes
    char compressaoForcada = 0;  // 1 se quisermos forçar compressão, senão 0
    //unsigned long tamanhoBloco = 0;
    moduloF(nomeFicheiro, compressaoForcada);


    return 0;
}


*****************************************************************************

>Esta invoca a função moduloF;

>moduloF invoca 2 funções:
	>NBlocos
    (Calculo do número de blocos)
    >calculoFrequencias
    (calcula as Frequencias)
A segunda depende de informação da primeira. 
Logo a ideia passaria por aplicar Multithreading ao cálculo das frequências


*****************************************************************************
*/


//A função que é executada pela thread
void* calculoFrequenciasBlocoTHREAD(void* argsThread){
	//Fazer o casting dos argumentos que a Thread recebe
	ArgsThread argumentos = (struct argsThread *)argsThread;

	//Info da struct necessária para executar a função:
	FILE *orig = argumentos->orig;
    FicheiroInf fInf = argumentos->fInf;
	unsigned long long numBloco = argumentos->numBloco;
	char compressaoForcada = argumentos->compressaoForcada;
	unsigned long long *tamanhoRleAcumulado = argumentos->tamanhoRleAcumulado;    

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



void calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada){
    //criação do ID das threads
    pthread_t id;

    unsigned long long numBloco;
    unsigned long long tamanhoRLE = 0;
   
    for(numBloco = 0; numBloco < fInf -> numBloco; numBloco++) {
    	//uma thread para blocos pares, outra para blocos impares
    	//CUIDADO COMO JUNTO AS INFOS!!!!!

    	ArgsThread argumentos = malloc(sizeof(struct argsThread));

    	argumentos-> orig = orig;
    	argumentos-> fInf = fInf;
    	argumentos-> numBloco = numBloco;
    	argumentos-> compressaoForcada = compressaoForcada;
    	argumentos-> tamanhoRleAcumulado = &tamanhoRLE;
        

        pthread_create(&id, NULL, calculoFrequenciasBlocoTHREAD, &argumentos);


        pthread_join(id,NULL); //Ainda não sei se será NULL; posso por a void a devolver algo....

        free(argumentos);
    }
}


