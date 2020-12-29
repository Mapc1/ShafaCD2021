

#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
char *nomeficheiro(char *tipoficheiro, FicheiroInf fInf) {

    int i=0;
    int j=0;
    int k=0;
    int tamanho_ficheiro = (int) (strlen(fInf -> nomeFicheiro) + strlen(tipoficheiro));
    char *nomeFicheiroExtensao = malloc(sizeof(char) * tamanho_ficheiro);
    while(i<tamanho_ficheiro){
        while((fInf -> nomeFicheiro)[j] != '\0') nomeFicheiroExtensao[i++] = (fInf -> nomeFicheiro)[j++];
        while(tipoficheiro[k] != '\0') nomeFicheiroExtensao[i++] = tipoficheiro[k++];
    }
    nomeFicheiroExtensao[i] = 0;

    return nomeFicheiroExtensao;
}
*/

char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao) {
	size_t length = strlen(nomeFicheiro) + strlen(extensao) + 1;
	char *concat = malloc(sizeof(char) * length);
	if (!concat) return NULL; // Malloc error
	snprintf(concat, length, "%s%s", nomeFicheiro, extensao);

	return concat;
}



FicheiroInf NBlocos(FILE *f, char *nomeFicheiro, unsigned long int tamanhoBloco) { // TamanhoBloco vem em Bytes!!!
    FicheiroInf fic = malloc(sizeof(struct ficheiroInf));
    fic -> tamanhoBloco = tamanhoBloco;
    fic -> nomeFicheiro = nomeFicheiro;
    fic -> numBloco = fsize(f, NULL, &(fic -> tamanhoBloco), &(fic -> tamanhoUltimoBloco));
    fic -> tamanhoTotal = (fic -> numBloco - 1) * (fic -> tamanhoBloco) + (fic -> tamanhoUltimoBloco);
    
    return fic;
}


Byte *Bloco_to_array(FILE *f, FicheiroInf fInf, unsigned long long int numBloco) {
    unsigned long long int tamanhoArray;
    // Verificar se o numBloco é o ultimo e meter um buffer com um possível espaço irregular em relação aos outros blocos
    if (numBloco == (fInf -> numBloco) - 1) tamanhoArray = fInf -> tamanhoUltimoBloco;
    else tamanhoArray = fInf -> tamanhoBloco;

    Byte *bloco = malloc(tamanhoArray * sizeof(Byte)); // Criação de um buffer para armazenar os blocos

    // Escrita do bloco no array
    fread(bloco,  1, tamanhoArray, f);

    return bloco;
}

void escrita_freqs(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, FicheiroRleInf RleInf, int compr) {
	unsigned long long int numBloco;

    fprintf(freqOrig, "@N@%lld", fInf->numBloco);

	if (compr == RLE_NAO) { // Não se faz a compressão RLE e apaga-se o ficheiro .rle
	    remove(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle"));
	    remove(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq"));
	    for (numBloco = 0; numBloco < fInf->numBloco; numBloco++) {
            frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, NULL, numBloco);
	    }
    } else {
	    // Compensa fazer RLE ou foi forçada
	    fprintf(freqRLE, "@R@%lld", fInf->numBloco);
	    for (numBloco = 0; numBloco < fInf->numBloco; numBloco++) {
            frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, RleInf, numBloco);
	    }
	    fprintf(freqRLE, "@0\n");
    }
    fprintf(freqOrig, "@0\n");
}

FicheiroRleInf compressaoRLE(FILE *orig, FicheiroInf fInf, FILE *rle, FILE *freqOrig, FILE *freqRLE, char compressaoForcada) {
    /* -> Fazer compressão para o bloco 1
     * --> Identificar padrões de 4 ou +
     * --> Identificar 0's
     * --> Aplicar padrão RLE de 3 Bytes para comprimir ou para escrever 0's
     * -> Ver se compensa fazer compressão rle
     * --> Se compensa, fazemos para o restantes blocos
     * --> Se nao compensa, apagamos o ficheiro .rle
     */

    unsigned long long int numBloco = 0;
    static struct ficheiro_RLE_Inf RleInf;

    // Compressão forçada
    if (compressaoForcada) {
        // -> Geração do ficheiro FREQ do ficheiro original e RLE
        escrita_freqs(orig, fInf, rle, freqOrig, freqRLE, &RleInf, RLE_SIM);
    }
    else {

        // Bloco 1
        frequencias_Bloco(orig, rle, fInf, freqOrig, freqRLE, &RleInf, numBloco);


        // Taxa de compressão RLE do primeiro bloco
        double compressao_bloco1 = (double)(fInf -> numBloco > 1 ? RleInf.tamanhoBlocoRleAcumulado:RleInf.tamanhoUltimoBlocoRle) / (double)fInf -> tamanhoBloco;
        if (compressao_bloco1 < 0.95) {
            escrita_freqs(orig, fInf, rle, freqOrig, freqRLE, NULL, RLE_NAO);
            return NULL;
        }
        else escrita_freqs(orig, fInf, rle, freqOrig, freqRLE, &RleInf, RLE_SIM);

    }
    return &RleInf;
}


// Quando compensa fazer RLE ou é forçado o RLE, esta função cria um array com freq dos dois ficheiros
// Else, a função frequencias, conta as frequencias por si mesma
FreqsInf compressaoRLEBloco(FILE *orig, FicheiroInf fInf, FILE *rle, FicheiroRleInf RleInf, unsigned long long int numBloco) { // Assume-se que está função será chamada com numBloco sucessivos...
    Byte *Buffer = Bloco_to_array(orig, fInf, numBloco);
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
    if (numBloco < fInf->numBloco) {
        tamanhoBlocoAtual = (fInf->tamanhoBloco);
    } else tamanhoBlocoAtual = fInf->tamanhoUltimoBloco;

    int i = 0;
    while (i < tamanhoBlocoAtual) { // Analisar um símbolo de cada vez
        Byte num_repeticoes = 1;
        Byte simbolo = Buffer[i];
        for (; i < tamanhoBlocoAtual; i++) { // Identificação de repetições;
            if (i == tamanhoBlocoAtual - 1 || (simbolo != Buffer[i + 1] || num_repeticoes == 255)) {
                i++;
                break;
            }
            else num_repeticoes++;
        }
        if (num_repeticoes >= 4 || simbolo == 0) { // O símbolo repete-se pelo menos 4 vezes ou o símbolo ser 0, logo aplicamos o padrão RLE
            Byte padraoRLE[3] = {0, simbolo, num_repeticoes}; // {0}{simbolo}{número_de_repetições}
            fwrite(padraoRLE, 3, 1, rle);

            // Contagem das frequências dos símbolos do ficheiro RLE
            BufferFreqRle[0]++;
            BufferFreqRle[simbolo]++;
            BufferFreqRle[num_repeticoes]++;

            // Contagem das frequências dos símbolos do ficheiro orig
            BufferFreqOrig[simbolo] += num_repeticoes;

            // Contagem do número de símbolos do ficheiro rle (Tamanho do ficheiro)
            if (!RleInf) continue;
            if (numBloco != fInf -> numBloco - 1) RleInf -> tamanhoBlocoRleAcumulado += 3;
            else RleInf -> tamanhoUltimoBlocoRle += 3;

        } else { // O símbolo não se repete pelo menos 4 vezes, logo não aplicamos o padrão RLE
            BufferFreqRle[simbolo] += num_repeticoes;
            BufferFreqOrig[simbolo] += num_repeticoes;
            for (; num_repeticoes > 0; num_repeticoes--) fwrite(&simbolo, 1, 1, rle);

            // Contagem do número de símbolos do ficheiro rle (Tamanho do ficheiro)
            if (!RleInf) continue;
            if (numBloco != fInf -> numBloco - 1) RleInf -> tamanhoBlocoRleAcumulado++;
            else RleInf -> tamanhoUltimoBlocoRle++;
        }
    }
    free(Buffer);

    freq -> FicheiroOriginal = BufferFreqOrig;
    freq -> FicheiroRLE = BufferFreqRle;

    return freq;
}


void frequencias_Bloco(FILE *orig, FILE *rle, FicheiroInf fInf, FILE *freqOrig, FILE *freqRLE, FicheiroRleInf RleInf, unsigned long long int numBloco) {
    unsigned long long int tamanhoBlocoAtual;
    int i;

    if (numBloco < fInf->numBloco - 1) {
        tamanhoBlocoAtual = (fInf->tamanhoBloco);
    } else tamanhoBlocoAtual = fInf->tamanhoUltimoBloco;

    fprintf(freqOrig, "%c%llu%c", '@', tamanhoBlocoAtual, '@');


    // ESCRITA DAS FREQUÊNCIAS NO FICHEIRO


    if (freqRLE) { // Saber se é para gerar o ficheiro FREQ dos ficheiros original e do RLE ou se é só do ficheiro original

        fprintf(freqRLE, "%c%llu%c", '@', tamanhoBlocoAtual, '@');
        FreqsInf aux_Freqs = compressaoRLEBloco(orig, fInf, rle, RleInf, numBloco); // Buffer com as frequências dos símbolos

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
        // Buffer com as frequências
        unsigned long long int BufferFreq[256] = {0}; // Inicializar o buffer a 0

        // Buffer com os símbolos
        Byte *Buffer = Bloco_to_array(orig, fInf, numBloco);

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


void ficheiros_gerados(FicheiroInf fInf, FicheiroRleInf RleInf) {
	char *freqs_Original = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".freq");
	char *RLE = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle");
	char *freqs_RLE = nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle.freq");
	printf("%s", freqs_Original);
	if (RleInf) printf(", %s, %s", RLE, freqs_RLE);

}


void data() {
    time_t t = time(NULL);
	struct tm tm = *localtime(&t);	
	printf("%02d-%02d-%d \n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}


int main() {
    // NAO ESQUECER: GENERALIZAR A MAIN!!!!!!!!!!!!!!!!


    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    char nomeFicheiro[30] = "teste_All1.txt"; // Argv[1] // Aumentado devido a alguns nomes dos testes

    // Abertura dos ficheiros
    FILE *orig;
    orig = fopen(nomeFicheiro,"rb"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }

    FicheiroInf fInf = NBlocos(orig, nomeFicheiro, 0);

    FILE *rle = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro, ".rle"),"wb"); // Ficheiro rle
    FILE *freqOrig = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro,".freq"),"w");
    FILE *freqRLE = fopen(nomeFicheiroExtensao(fInf -> nomeFicheiro,".rle.freq"),"w");

    // CompressãoRLE
    char compressaoForcada = 0;  // 1 se quisermos forçar compressão, senão 0
    FicheiroRleInf RleInf = compressaoRLE(orig, fInf, rle, freqOrig, freqRLE, compressaoForcada);
    
    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Informações a aparecer na consola:
    printf("Miguel Martins, a93280, Gonçalo Soares, a93286, MIEI/CD, ");
    data();
    printf("Módulo: f (cálculo das frequências dos símbolos)\n");
    printf("Número de blocos: %llu\n", fInf -> numBloco);
    if (fInf -> numBloco > 1) printf("Tamanho dos blocos analisados no ficheiro original: %lu/%lu\n", fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco);
    else printf("Tamanho do bloco analisado no ficheiro original: %lu\n", fInf -> tamanhoUltimoBloco);
    if (!RleInf) {
        printf("Compressão RLE: Não efetuada\n");
    } else {
        double TaxaCompressaoMedia = (double) (RleInf -> tamanhoBlocoRleAcumulado + RleInf -> tamanhoUltimoBlocoRle) / (double) fInf -> tamanhoTotal;
        printf("Compressão RLE: %s.rle (%lf%% compressão)\n", fInf->nomeFicheiro, (TaxaCompressaoMedia > 1 ? 0 : ((1 - TaxaCompressaoMedia) * 100)));
        if (fInf -> numBloco > 1) printf("Tamanho dos blocos analisados no ficheiro RLE: %llu/%llu\n", RleInf -> tamanhoBlocoRleAcumulado, RleInf -> tamanhoUltimoBlocoRle);
        else printf("Tamanho do bloco analisado no ficheiro RLE: %llu\n", RleInf -> tamanhoUltimoBlocoRle);
    }
    printf("Tempo de execução do módulo: %f milisegundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000);
    printf("Ficheiros gerados: ");
    ficheiros_gerados(fInf, RleInf);

    // Fechar os ficheiros
    fclose(orig);
    fclose(rle);
    fclose(freqOrig);
    fclose(freqRLE);

    return 0;
}
