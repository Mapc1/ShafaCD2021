#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "threading.h"
#include "funcoesAuxiliares.h"
#include "processamento.h"


int main() {
    char nomeFicheiro[30] = "Shakespeare.txt"; // Argv[1] // Aumentado devido a alguns nomes dos testes
    char compressaoForcada = 0;  // 1 se quisermos forçar compressão, senão 0
    unsigned long tamanhoBloco = 0; //8388608;
    moduloF(nomeFicheiro, compressaoForcada, tamanhoBloco);
    return 0;
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
    FILE *orig = fopen(nomeFicheiro, "r"); // Ficheiro original

    unsigned long long tamanhoRle = calculoFrequenciasTHREAD(orig, fInf, compressaoForcada);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();


    // Informações a aparecer na consola:
    infoTerminal(fInf, tamanhoRle, inicio, fim);

    // Free fInf
    freeFicheiroInf(fInf);

    fclose(orig);
    return 0;
}

//A função que é executada pela thread
void* calculoFrequenciasBlocoTHREAD(void* argsThread){

    pthread_mutex_t lock;

	pthread_mutex_lock(&lock); 

	//Fazer o casting dos argumentos que a Thread recebe
	ArgsThread argumentos = (struct argsThread *)argsThread;

	//Info da struct necessária para executar a função:
	FILE *orig = argumentos->orig;
    FicheiroInf fInf = argumentos->fInf;
	unsigned long long numBloco = argumentos->numBloco;
	unsigned long long *tamanhoRleAcumulado = argumentos->tamanhoRleAcumulado;    

	// Leitura do bufferInput
    Byte *bufferInput = leituraFicheiro(orig, tamanhoBloco(fInf, numBloco));
    InfosBloco infosBloco;

    // Calculamos as frequências do rle ou do original?
    // Calculamos a compressão rle do primeiro bloco
    // Se tamanhoBlocoRle / tamanhoBlocoOriginal < 0.95, efetuamos a compressao rle

    infosBloco = processamento(bufferInput, fInf, numBloco, tamanhoRleAcumulado);

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


    pthread_mutex_unlock(&lock);
    
}



unsigned long long int calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada){
    //criação do ID das threads
    pthread_t t1, t2;  //serão usadas 2 threads, uma para os blocos pares,outra para os impares
    				   //t1: ------
    				   //t2: ------
    pthread_mutex_t lock; 

    unsigned long long numBloco = 0;
    unsigned long long tamanhoRLE = 0;
    unsigned long long *tamanhoRLEPointer = &tamanhoRLE;
    
    // se compr forçada tudo threading, senao primeiro do bloco 0 e depois do resto
    if(compressaoForcada){





        for(numBloco = 0; numBloco < fInf -> numBloco; numBloco = numBloco + 2) {
            //uma thread para blocos pares, outra para blocos impares
            //CUIDADO COMO JUNTO AS INFOS!!!!!

            ArgsThread argumentosPar = malloc(sizeof(struct argsThread));
            ArgsThread argumentosImpar = malloc(sizeof(struct argsThread));

            argumentosPar-> orig = orig;
            argumentosPar-> fInf = fInf;
            argumentosPar-> numBloco = numBloco;
            argumentosPar-> tamanhoRleAcumulado = tamanhoRLEPointer;
     
            argumentosImpar-> orig = orig;
            argumentosImpar-> fInf = fInf;
            argumentosImpar-> numBloco = numBloco+1;
            argumentosImpar-> tamanhoRleAcumulado = tamanhoRLEPointer;
        

            pthread_create(&t1, NULL, calculoFrequenciasBlocoTHREAD, &argumentosPar);
            pthread_create(&t2, NULL, calculoFrequenciasBlocoTHREAD, &argumentosImpar);

            pthread_join(t1,NULL); //Ainda não sei se será NULL; posso por a void a devolver algo....
            pthread_join(t2,NULL);

            free(argumentosPar);
            free(argumentosImpar);
        }
    }else{
            unsigned long long numBloco = 0;
            unsigned long long tamanhoRLE = 0;
            calculoFrequenciasBloco1(orig,fInf,numBloco,compressaoForcada,&tamanhoRLEPointer);
            numBloco ++;

            for(numBloco = 1; numBloco < fInf -> numBloco; numBloco = numBloco + 2) {
            //uma thread para blocos pares, outra para blocos impares
            //CUIDADO COMO JUNTO AS INFOS!!!!!

            ArgsThread argumentosImpar = malloc(sizeof(struct argsThread));
            ArgsThread argumentosPar = malloc(sizeof(struct argsThread));

            argumentosImpar-> orig = orig;
            argumentosImpar-> fInf = fInf;
            argumentosImpar-> numBloco = numBloco;
            argumentosImpar-> tamanhoRleAcumulado = tamanhoRLEPointer;
     
            argumentosPar-> orig = orig;
            argumentosPar-> fInf = fInf;
            argumentosPar-> numBloco = numBloco+1;
            argumentosPar-> tamanhoRleAcumulado = tamanhoRLEPointer;
        

            pthread_create(&t1, NULL, calculoFrequenciasBlocoTHREAD, &argumentosImpar);
            pthread_create(&t2, NULL, calculoFrequenciasBlocoTHREAD, &argumentosPar);

            pthread_join(t1,NULL); //Ainda não sei se será NULL; posso por a void a devolver algo....
            pthread_join(t2,NULL);

            free(argumentosImpar);
            free(argumentosPar);
        }


    }

    pthread_mutex_destroy(&lock);
    return tamanhoRLE;

}



void calculoFrequenciasBloco1(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado) {
    // Leitura do bufferInput
    Byte *bufferInput = leituraFicheiro(orig, tamanhoBloco(fInf, numBloco));
    InfosBloco infosBloco;

    // Calculamos as frequências do rle ou do original?
    // Calculamos a compressão rle do primeiro bloco
    // Se tamanhoBlocoRle / tamanhoBlocoOriginal < 0.95, efetuamos a compressao rle

    // Primeiro bloco
     
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


    //Libertar espaço dos buffer
    free(bufferInput);
    libertarEspacoInfosBloco(infosBloco);


}
