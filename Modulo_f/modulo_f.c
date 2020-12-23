

#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *novoficheiro(char *tipoficheiro, FicheiroInf fInf) {

    int i=0;
    int j=0;
    int k=0;
    int tamanho_ficheiro = (int) (strlen(fInf -> nomeFicheiro) + strlen(tipoficheiro));
    char *novoficheiro = malloc(sizeof(char) * tamanho_ficheiro);
    while(i<tamanho_ficheiro){
        while((fInf -> nomeFicheiro)[j] != '\0') novoficheiro[i++] = (fInf -> nomeFicheiro)[j++];
        while(tipoficheiro[k] != '\0') novoficheiro[i++] = tipoficheiro[k++];
    }
    return novoficheiro;
}

FicheiroInf NBlocos(FILE *f, unsigned long long int tamanhoBloco, unsigned long long int tamanhoMinimoUltimoBloco, char *nomeFicheiro) { // TamanhoBloco vem em Bytes!!!
    FicheiroInf fic = malloc(sizeof(struct ficheiroInf));
    fic -> nomeFicheiro = nomeFicheiro;
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


unsigned char *Bloco_to_array(FILE *f, FicheiroInf fInf, unsigned long long int num_bloco) {
    unsigned long long int tamanhoArray;
    // Verificar se o num_bloco é o ultimo e meter um buffer com um possível espaço irregular em relação aos outros blocos
    if (num_bloco == (fInf -> num_blocos) - 1) tamanhoArray = fInf -> tamanhoUltimoBloco;
    else tamanhoArray = fInf -> tamanhoBloco;

    unsigned char *bloco = malloc(tamanhoArray * sizeof(unsigned char)); // Criação de um buffer para armazenar os blocos

    fseek(f, (long) ((unsigned long long int) num_bloco * (fInf -> tamanhoBloco)), SEEK_SET); // Colocação do apontador de posição no inicio do bloco

    // Escrita do bloco no array
    fread(bloco,  1, tamanhoArray, f);

    return bloco;
}

void escrita_freqs(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, int compr) {

	unsigned long long int num_bloco;

	if (compr == RLE_NAO) { // Não se faz a compressão RLE e apaga-se o ficheiro .rle
	    remove(novoficheiro(".rle", fInf));
	    remove(novoficheiro(".rle.freq", fInf));
	    fprintf(freqOrig, "@N@%lld", fInf->num_blocos);
	    for (num_bloco = 0; num_bloco < fInf->num_blocos; num_bloco++) {
	        frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, num_bloco);
	    }
	    fprintf(freqOrig, "@0\n");
            
    } else {
	    // Compensa fazer RLE ou foi forçada
	    fprintf(freqOrig, "@N@%lld", fInf->num_blocos);
	    fprintf(freqRLE, "@R@%lld", fInf->num_blocos);
	    for (num_bloco = 0; num_bloco < fInf->num_blocos; num_bloco++) {
	        frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, num_bloco);
	    }
	    fprintf(freqOrig, "@0\n");
	    fprintf(freqRLE, "@0\n");
    }

}

void compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, char compressaoForcada) {
    /* -> Fazer compressão para o bloco 1
     * --> Identificar padrões de 4 ou +
     * --> Identificar 0's
     * --> Aplicar padrão RLE de 3 Bytes para comprimir ou para escrever 0's
     * -> Ver se compensa fazer compressão rle
     * --> Se compensa, fazemos para o restantes blocos
     * --> Se nao compensa, apagamos o ficheiro .rle
     */

    unsigned long long int num_bloco = 0;

    // Compressão forçada
    if (compressaoForcada) {
        // -> Geração do ficheiro FREQ do ficheiro original e RLE
    	escrita_freqs(orig, fInf,rle, freqOrig, freqRLE, RLE_SIM);
    }
    else {

        // Bloco 1
        frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, num_bloco);

        // Taxa de compressão RLE do primeiro bloco
        double compressao_bloco1 = (double)tamanhoFicheiro(rle) / (double)fInf -> tamanhoBloco;
        if (compressao_bloco1 > 0.95) { // Não se faz a compressão RLE e apaga-se o ficheiro .rle
            escrita_freqs(orig, fInf,rle, freqOrig, freqRLE, RLE_NAO);
        }
        else{ // Compensa fazer a compressão RLE
        	escrita_freqs(orig, fInf,rle, freqOrig, freqRLE, RLE_SIM);
        }
    }
}


// Quando compensa fazer RLE ou é forçado o RLE, esta função cria um array com freq dos dois ficheiros
// Else, a função frequencias, conta as frequencias por si mesma
FreqsInf compressaoRLEBloco (FILE *orig, FicheiroInf fInf, FILE *rle, unsigned long long int num_bloco) { // Assume-se que está função será chamada com num_bloco sucessivos...
    unsigned char *Buffer = Bloco_to_array(orig, fInf, num_bloco);
    unsigned long long int tamanhoBlocoAtual;

    FreqsInf freq = malloc(sizeof (struct freqsInf));
    unsigned long long int *BufferFreqOrig = malloc(sizeof (unsigned long long int) * 256);
    unsigned long long int *BufferFreqRle = malloc(sizeof (unsigned long long int) * 256);

    // Inicializar o buffer a 0
    int j;
    for(j = 0; j < 256; j++) {
        BufferFreqOrig[j] = 0;
        BufferFreqRle[j] = 0;
    }

    // Tamanho do bloco
    if (num_bloco < fInf->num_blocos) {
        tamanhoBlocoAtual = (fInf->tamanhoBloco);
    } else tamanhoBlocoAtual = fInf->tamanhoUltimoBloco;

    // Mudar para for para meter continue !!!!!!!!!!!!!!!!!!!
    int i = 0;
    while (i < tamanhoBlocoAtual) { // Analisar um símbolo de cada vez
        unsigned char num_repeticoes = 1;
        unsigned char simbolo = Buffer[i];
        for (; i < tamanhoBlocoAtual; i++) { // Identificação de repetições;
            if (i == tamanhoBlocoAtual - 1 || (simbolo != Buffer[i + 1] || num_repeticoes == 255)) {
                i++;
                break;
            }
            else num_repeticoes++;
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
            BufferFreqRle[simbolo] += num_repeticoes;
            BufferFreqOrig[simbolo] += num_repeticoes;
            for (; num_repeticoes > 0; num_repeticoes--) fwrite(&simbolo, 1, 1, rle);
        }
    }
    free(Buffer);

    freq -> FicheiroOriginal = BufferFreqOrig;
    freq -> FicheiroRLE = BufferFreqRle;

    return freq;
}


void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freqOrig, FILE *freqRLE, unsigned long long int numBloco) {
    unsigned long long int tamanhoBlocoAtual;
    int i;

    if (numBloco < fInf->num_blocos - 1) {
        tamanhoBlocoAtual = (fInf->tamanhoBloco);
    } else tamanhoBlocoAtual = fInf->tamanhoUltimoBloco;

    fprintf(freqOrig, "%c%llu%c", '@', tamanhoBlocoAtual, '@');


    // ESCRITA DAS FREQUÊNCIAS NO FICHEIRO


    if (freqRLE) { // Saber se é para gerar o ficheiro FREQ dos ficheiros original e do RLE ou se é só do ficheiro original

        fprintf(freqRLE, "%c%llu%c", '@', tamanhoBlocoAtual, '@');
        FreqsInf aux_Freqs = compressaoRLEBloco(orig, fInf, rle, numBloco); // Buffer com as frequências dos símbolos

        // -> Geração do ficheiro FREQ do ficheiro original
        for (i = 0; i < 256; i++) { // Otimizar condições!!!!!
            if (!i || ((aux_Freqs->FicheiroOriginal[i] != aux_Freqs->FicheiroOriginal[i - 1]))) {
                fprintf(freqOrig, "%lld;", (aux_Freqs->FicheiroOriginal[i]));
                continue;
            } else if (aux_Freqs->FicheiroOriginal[i] == aux_Freqs->FicheiroOriginal[i - 1]) fprintf(freqOrig, ";");
            else if (i != 255) fprintf(freqOrig, "%lld", (aux_Freqs->FicheiroOriginal[i]));
            else
                fprintf(freqOrig, "%lld;",(aux_Freqs->FicheiroOriginal[i])); // (aux_Freqs->FicheiroOriginal[i] != aux_Freqs->FicheiroOriginal[i-1])
        }

        // -> Geração do ficheiro FREQ do ficheiro RLE
        for (i = 0; i < 256; i++) {
            if (!i || ((aux_Freqs->FicheiroRLE[i] != aux_Freqs->FicheiroRLE[i - 1]))) {
                fprintf(freqRLE, "%lld;", (aux_Freqs->FicheiroRLE[i]));
                continue;
            } else if (aux_Freqs->FicheiroRLE[i] == aux_Freqs->FicheiroRLE[i - 1]) fprintf(freqRLE, ";");
            else if (i != 255) fprintf(freqRLE, "%lld", (aux_Freqs->FicheiroRLE[i]));
            else
                fprintf(freqRLE, "%lld;",(aux_Freqs->FicheiroRLE[i])); // (aux_Freqs->FicheiroRLE[i] != aux_Freqs->FicheiroRLE[i-1])
        }
    } else { // Geração do ficheiro FREQ do original e contagem dos símbolos
        // OTIMIZAR C O PRIMEIRO BLOCO !!!!!!!!!!!

        // Buffer com as frequências
        unsigned long long int BufferFreq[256] = {0}; // Inicializar o buffer a 0

        // Buffer com os símbolos
        unsigned char *Buffer = Bloco_to_array(orig, fInf, numBloco);

        // -> Contagem de símbolos
        for (i = 0; i < tamanhoBlocoAtual; i++) {
            BufferFreq[Buffer[i]]++;
        }

        // -> Escrita das frequências dos símbolos no ficheiro FREQ
        for (i = 0; i < 256; i++) { // Otimizar condições!!!!!
            if (!i || ((BufferFreq[i] != BufferFreq[i - 1]))) {
                fprintf(freqOrig, "%lld;", (BufferFreq[i]));
                continue;
            } else if (BufferFreq[i] == BufferFreq[i - 1]) fprintf(freqOrig, ";");
            else if (i != 255) fprintf(freqOrig, "%lld", (BufferFreq[i]));
            else
                fprintf(freqOrig, "%lld;",(BufferFreq[i])); // (aux_Freqs->FicheiroOriginal[i] != aux_Freqs->FicheiroOriginal[i-1])
        }
    }
}


void ficheiros_gerados(FILE *rle, FILE *freqOrig, FILE *freqRLE, FicheiroInf fInf){
	char *freqs_Original = novoficheiro(".freq", fInf); 
	char *fich_RLE = novoficheiro(".rle", fInf);
	char *freqs_fich_RLE = novoficheiro(".rle.freq", fInf);
	if(freqOrig) printf("%s,",freqs_Original);
	if(rle) printf(" %s,",fich_RLE); //??? ver dúvida da main
	if(freqRLE) printf(" %s\n",freqs_fich_RLE);

}



int main() {
    // NAO ESQUECER: GENERALIZAR A MAIN!!!!!!!!!!!!!!!!


    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Abertura dos ficheiros
    char nomeFicheiro[30] = "teste_All1.txt"; // Argv[1] //Aumentado devido a alguns nomes dos testes

    FILE *orig;
    orig = fopen(nomeFicheiro,"rb"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }

    FicheiroInf fInf = NBlocos(orig, TAMANHO_BLOCO, TAMANHO_MINIMO_ULTIMO_BLOCO, nomeFicheiro);

    FILE *rle = fopen(novoficheiro(".rle", fInf),"wb"); // Ficheiro rle
    FILE *freqOrig = fopen(novoficheiro(".freq", fInf),"w");
    FILE *freqRLE = fopen(novoficheiro(".rle.freq", fInf),"w");


    // CompressãoRLE
    char compressaoForcada = 1;  //1 se quisermos forçar compressão, senão 2 
    compressaoRLE(orig, fInf, rle, freqOrig, freqRLE, compressaoForcada);


    // Fim da contagem do tempo de execução
    clock_t fim = clock();



    // Informações a aparecer na consola:

    printf("Miguel Martins, a93280, Gonçalo Soares, a93286, MIEI/CD, data\n");
    printf("Módulo: f (cálculo das frequências dos símbolos)\n");
    printf("Número de blocos: %llu\n", fInf -> num_blocos);
    printf("Tamanho dos blocos analisados no ficheiro original: %llu/%llu\n", fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco);
    double TaxaCompressao = (double)tamanhoFicheiro(rle) / (double)fInf -> tamanhoTotal;
    printf("Compressão RLE: %s.rle (%lf%% compressão)\n", fInf -> nomeFicheiro, TaxaCompressao);
    printf("Tamanho dos blocos analisados no ficheiro RLE: 57444/1620 bytes\n"); // Ver isto dps!!!!!!!!!!!
    printf("Tempo de execução do módulo: %f milisegundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000);
    printf("Ficheiros gerados: ");
    ficheiros_gerados(rle, freqOrig, freqRLE, fInf);
    //DÚVIDA: A indicação de que o ficheiro RLE também foi gerado(se for gerado), também deve aparecer na consola?????




    // Fechar os ficheiros
    fclose(orig);
    fclose(rle);
    fclose(freqOrig);
    fclose(freqRLE);

    return 0;
}