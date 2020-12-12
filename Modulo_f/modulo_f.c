

#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FicheiroInf NBlocos(FILE *f, unsigned long long int tamanhoBloco, unsigned long long int tamanhoMinimoUltimoBloco) { // tamanhoBloco vem em Bytes!!!
    FicheiroInf fic = malloc(sizeof(struct ficheiroInf));
    
    fic -> tamanhoTotal = tamanhoFicheiro(f);
    unsigned long long int resto = (fic -> tamanhoTotal) % (tamanhoBloco);

    if (fic -> tamanhoTotal <= tamanhoBloco + tamanhoMinimoUltimoBloco) { // Só temos um bloco
        fic -> num_blocos = 1;
        fic -> tamanhoUltimoBloco = fic -> tamanhoBloco = fic -> tamanhoTotal;
    } else {
        fic->num_blocos = (int) (fic->tamanhoTotal / tamanhoBloco);

        if (resto >= tamanhoMinimoUltimoBloco) {
            fic -> tamanhoBloco++;
            fic -> tamanhoUltimoBloco = resto;
        } else {
            fic -> tamanhoBloco = tamanhoBloco;
            fic -> tamanhoUltimoBloco = fic -> tamanhoBloco + resto;
        }
    }

    return fic;
}

unsigned long long int tamanhoFicheiro (FILE *f) {
   if (!f) return 0;
   else {
      fseek(f, 0L, SEEK_END);
      unsigned long long int tamanho = ftell(f);
      return tamanho;
   }
} // Inclui o caracter  ?? \r ??


char *Bloco_to_array(FILE *f, FicheiroInf fInf, unsigned long long int num_bloco) {
    unsigned long long int tamanhoArray;
    // Verificar se o num_bloco é o ultimo e meter um buffer com um possível espaço irregular em relação aos outros blocos
    if (num_bloco == (fInf -> num_blocos) - 1) tamanhoArray = fInf -> tamanhoUltimoBloco;
    else tamanhoArray = fInf -> tamanhoBloco;

    char *bloco = malloc(tamanhoArray * sizeof(char)); // Criação de um buffer para armazenar os blocos

    fseek(f, (long) ((unsigned long long int) num_bloco * (fInf -> tamanhoBloco)), SEEK_SET); // Colocação do apontador de posição no inicio do bloco

    // Escrita do bloco no array
    fread(bloco,  1, tamanhoArray, f);

    return bloco;
}

double compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, char compressaoForcada) {
    /* -> Fazer compressão para o bloco 1
     * --> Identificar padrões de 4 ou +
     * --> Identificar 0's
     * --> Aplicar padrão RLE de 3 Bytes para comprimir ou para escrever 0's
     * -> Ver se compensa fazer compressão rle
     * --> Se compensa, fazemos para o restantes blocos
     * --> Se nao compensa, apagamos o ficheiro .rle
     * -> Retorna a taxa de compressão (?????????)
     */

    unsigned long long int num_bloco = 0;

    // Compressão forçada

    if (compressaoForcada) for (num_bloco = 0; num_bloco < fInf -> num_blocos ; num_bloco++) compressaoRLEBloco(orig, fInf, rle, num_bloco);

    else {

        // Bloco 1
        compressaoRLEBloco(orig, fInf, rle, num_bloco);

        // Taxa de compressão RLE do primeiro bloco
        double compressao_bloco1 = (double)tamanhoFicheiro(rle) / (double)fInf -> tamanhoBloco;
        printf("Taxa de compressão do primeiro bloco: %lf\n", compressao_bloco1);
        if (compressao_bloco1 > 0.95) { // Não se faz a compressão RLE e apaga-se o ficheiro .rle
            remove("aaa.txt.rle");
            return 1; // A taxa de compressão é de 1, logo concluimos que não se fez a compressão RLE
        }


        // Visto que compensa fazer a compressão rle para o primeiro bloco, fazemos para os restantes.

        for (num_bloco = 1; num_bloco < fInf->num_blocos; num_bloco++) {
            compressaoRLEBloco(orig, fInf, rle, num_bloco);
        }
    }

    double TaxaCompressao = (double)tamanhoFicheiro(rle) / (double)fInf -> tamanhoTotal;
    printf("Taxa de compressão: %lf\n", TaxaCompressao);

    return TaxaCompressao;
}


// Quando compensa fazer RLE ou é forçado o RLE, esta função cria um array com freq dos dois ficheiros
// Else, a função frequencias, conta as frequencias por si mesma
FreqsInf compressaoRLEBloco (FILE *orig, FicheiroInf fInf, FILE *rle, unsigned long long int num_bloco) { // Assume-se que está função será chamada com num_bloco sucessivos...
    char *Buffer = Bloco_to_array(orig, fInf, num_bloco);
    int i = 0;
    static struct freqsInf freq;
    unsigned long long int static BufferFreqOrig[256];
    unsigned long long int static BufferFreqRle[256];

    while (i < fInf -> tamanhoBloco) { // Analisar um símbolo de cada vez
        unsigned char num_repeticoes = 1;
        unsigned char simbolo = Buffer[i];
        while (i < (fInf -> tamanhoBloco)) { // Identificação de repetições;
            if (i == (fInf->tamanhoBloco) - 1 || (simbolo != Buffer[i + 1] || num_repeticoes == 255)) {
                i++; // Para sair do ciclo externo
                break; // Para sair do ciclo interno
            }
            else {
                num_repeticoes++;
                i++;
            }
        }
        if (num_repeticoes >= 4 || simbolo == 0) { // O símbolo repete-se pelo menos 4 vezes ou o símbolo ser 0, logo aplicamos o padrão RLE
            unsigned char padraoRLE[3] = {0, simbolo, num_repeticoes}; // {0}{simbolo}{número_de_repetições}
            fwrite(padraoRLE, 3, 1, rle);
            // Contagem das frequências dos símbolos do ficheiro RLE
            BufferFreqRle[0]++;
            BufferFreqRle[simbolo]++;
            BufferFreqRle[num_repeticoes]++;
            // Contagem das frequências dos símbolos do ficheiro orig
            BufferFreqOrig[simbolo] += num_repeticoes;
        } else { // O símbolo não se repete pelo menos 4 vezes, logo não aplicamos o padrão RLE
            for (; num_repeticoes > 0; num_repeticoes--) fwrite(&simbolo, 1, 1, rle);
            BufferFreqRle[simbolo] += num_repeticoes;
            BufferFreqOrig[simbolo] += num_repeticoes;
        }
    }
    free(Buffer);

    freq.FicheiroOriginal = &(BufferFreqOrig[256]);
    freq.FicheiroRLE = &(BufferFreqRle[256]);

    return &freq;
}


void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freq, char tipoFicheiro, unsigned long long int numBloco){
    unsigned long long int tamanhoBlocoAtual;
    FreqsInf aux_Freqs =  malloc(sizeof(struct freqsInf)); 
    aux_Freqs = compressaoRLEBloco (orig, fInf, rle,numBloco);
    /*char *Buffer ;
    long int freq_Simbolos[256]={0}; //inicializar frequências a 0*/
    int i;

    if (numBloco < (fInf-> num_blocos)){
        tamanhoBlocoAtual = (fInf-> tamanhoBloco);
    }else tamanhoBlocoAtual = (fInf-> tamanhoUltimoBloco);

    /*if (tipoFicheiro == 'N'){
        Buffer = Bloco_to_array(orig, fInf, numBloco);
    }else if (tipoFicheiro == 'R') Buffer = Bloco_to_array(rle, fInf, numBloco);

    //ANÁLISE DAS FREQUÊNCIAS //ALGO ESTÁ MAL AQUI
    /*for(i = 0; i< 256; i++){
    	for(j = 0; j< tamanhoBlocoAtual -1; j++){
    		unsigned char buff = Buffer[j];
    		if(buff == i+'a') freq_Simbolos[i] ++;  //dúvidas nesta análise....somar 'a' ????
    	}
    }*/

	
    fprintf(freq, "%c%llu%c", '@', tamanhoBlocoAtual , '@');


    //ESCRITA DAS FREQUÊNCIAS NO FICHEIRO
    if (tipoFicheiro == 'N'){
    	for(i = 0; i< 256; i++){
			fprintf(freq, "%c", ';');
			if ((aux_Freqs->FicheiroOriginal[i]) > 0 ) fprintf(freq, "%llu", &(aux_Freqs -> FicheiroOriginal[i]));
    	}
    }
    else if (tipoFicheiro == 'R'){
    	for(i = 0; i< 256; i++){
			fprintf(freq, "%c", ';');
			if ((aux_Freqs->FicheiroRLE[i]) > 0 ) fprintf(freq, "%llu", &(aux_Freqs -> FicheiroRLE[i]));
    	}
    }

	//free(Buffer);

}


void frequencias(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freq,char tipoFicheiro){     //tipoFicheiro = 'N' || tipoFicheiro = 'R' quando a função é invocada 
    unsigned long long int bloco;

    fprintf(freq, "%c%c%c%c", '@', tipoFicheiro, '@', '0'+((int)fInf->num_blocos)); //as informações iniciais de um ficheiro freq são sempre iguais;
                                                                                    //soma-se '0' para converter o int num char
    
    for (bloco = 1; bloco <= (fInf-> num_blocos); bloco ++ ){
        frequencias_Bloco(orig,rle,fInf,freq,tipoFicheiro,bloco);
	}


    fprintf(freq, "%s\n", "@0");

}



int main() {
    // NAO ESQUECER: GENERALIZAR A MAIN!!!!!!!!!!!!!!!!

    // Generalizar o nome dos ficheiros!!!!!!!!!!!!

    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Abertura dos ficheiros
    FILE *orig;
    orig = fopen("aaa.txt","rb"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }

    FILE *rle = fopen("aaa.txt.rle","wb"); // Ficheiro rle
    FILE *freq = fopen("aaa.txt.freq","w"); // Ficheiro freq: Acho que não precisa de ser escrito em Binário
                                            //CONFIRMAR ISTO!!!
    										//NÃO É SEMPRE ASSIM QUE É GERADO : VER DISTINÇÃO ENTRE .freq e .rle.freq  !!!!!!!!!!!


    FicheiroInf fInf = NBlocos(orig, TAMANHO_BLOCO, TAMANHO_MINIMO_ULTIMO_BLOCO);
    printf("TamanhoTotal: %llu\nTamanhoBloco: %llu\nTamanhoUltimoBloco: %llu\nNum_Blocos: %lld \n", fInf -> tamanhoTotal, fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco, fInf -> num_blocos);

    // CompressãoRLE

    frequencias(orig,rle,fInf,freq,'N');

    // Fechar os ficheiros

    fclose(orig);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Impressão no terminal das informações sobre este módulo
    printf("Tempo de execução: %f segundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC); //O TEMPO DE EXECUÇÃO TERÁ DE SER APRESENTADO EM ms
    return 0;
}






   
