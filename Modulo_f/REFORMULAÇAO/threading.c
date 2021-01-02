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
	pthread_mutex_lock(&lock);

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


    pthread_mutex_unlock(&lock)
    
}



void calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada){
    //criação do ID das threads
    pthread_t t1, t2;  //serão usadas 2 threads, uma para os blocos pares,outra para os impares
    				   //t1: Pares
    				   //t2: Impares
    pthread_mutex_t lock; 


    unsigned long long numBloco;
    unsigned long long tamanhoRLE = 0;
	
	//se tivermos um número par de blocos, o ciclo for é efetuado sem considerações especiais
	//Ainda não tenho a certeza se este if é necessário!!!
	//VERIFICAR!!!!!
    if((fInf -> numBloco)%2 == 0){
	    for(numBloco = 0; numBloco < fInf -> numBloco; numBloco = numBloco + 2) {
    	    //uma thread para blocos pares, outra para blocos impares
    	    //CUIDADO COMO JUNTO AS INFOS!!!!!

    	   	ArgsThread argumentosPar = malloc(sizeof(struct argsThread));
	    	ArgsThread argumentosImpar = malloc(sizeof(struct argsThread));

    	    argumentosPar-> orig = orig;
            argumentosPar-> fInf = fInf;
    	    argumentosPar-> numBloco = numBloco;
    		argumentosPar-> compressaoForcada = compressaoForcada;
    		argumentosPar-> tamanhoRleAcumulado = &tamanhoRLE;
	 
    		argumentosImpar-> orig = orig;
    		argumentosImpar-> fInf = fInf;
    		argumentosImpar-> numBloco = numBloco+1;
    		argumentosImpar-> compressaoForcada = compressaoForcada;
    		argumentosImpar-> tamanhoRleAcumulado = &tamanhoRLE;
        

       	    pthread_create(&t1, NULL, calculoFrequenciasBlocoTHREAD, &argumentosPar);
       	    pthread_create(&t2, NULL, calculoFrequenciasBlocoTHREAD, &argumentosImpar);

        	pthread_join(t1,NULL); //Ainda não sei se será NULL; posso por a void a devolver algo....
        	pthread_join(t2,NULL);

	        free(argumentosPar);
		    free(argumentosImpar);
    	}
    }else{ //Senão fazemos por partes
	      for(numBloco = 0; numBloco < (fInf -> numBloco)-1; numBloco = numBloco + 2) {
    	    
      	   	    ArgsThread argumentosPar = malloc(sizeof(struct argsThread));
	    	    ArgsThread argumentosImpar = malloc(sizeof(struct argsThread));

    	    	argumentosPar-> orig = orig;
            	argumentosPar-> fInf = fInf;
    	    	argumentosPar-> numBloco = numBloco;
    			argumentosPar-> compressaoForcada = compressaoForcada;
    			argumentosPar-> tamanhoRleAcumulado = &tamanhoRLE;
	 
    			argumentosImpar-> orig = orig;
    			argumentosImpar-> fInf = fInf;
    			argumentosImpar-> numBloco = numBloco+1;
    			argumentosImpar-> compressaoForcada = compressaoForcada;
    			argumentosImpar-> tamanhoRleAcumulado = &tamanhoRLE;
        

       	        pthread_create(&t1, NULL, calculoFrequenciasBlocoTHREAD, &argumentosPar);
       	        pthread_create(&t2, NULL, calculoFrequenciasBlocoTHREAD, &argumentosImpar);

        		pthread_join(t1,NULL); //Ainda não sei se será NULL; posso por a void a devolver algo....
        							   //Talvez o tamanhoRLEacumulado
        		pthread_join(t2,NULL);

	        	free(argumentosPar);
				free(argumentosImpar);
    	  }
	      ArgsThread argumentosPar = malloc(sizeof(struct argsThread));
	    
	      argumentosPar-> orig = orig;
          argumentosPar-> fInf = fInf;
    	  argumentosPar-> numBloco = numBloco;
    	  argumentosPar-> compressaoForcada = compressaoForcada;
    	  argumentosPar-> tamanhoRleAcumulado = &tamanhoRLE;
	    
	      pthread_create(&t1, NULL, calculoFrequenciasBlocoTHREAD, &argumentos);
	      pthread_join(t1,NULL);
	      free(argumentosPar);
	     
	    }


	    pthread_mutex_destroy(&lock);


}