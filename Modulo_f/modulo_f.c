

#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FicheiroInf NBlocos(FILE *f, size_t tamanhoBloco, size_t tamanhoMinimoUltimoBloco) { // tamanhoBloco vem em Bytes!!!
    FicheiroInf fic = malloc(sizeof(struct ficheiroInf));
    
    fic -> tamanhoTotal = tamanhoFicheiro(f);
    size_t resto = ( fic -> tamanhoTotal) % (tamanhoBloco);

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

size_t tamanhoFicheiro (FILE *f) {
   if (!f) return 0;
   else {
      fseek(f, 0L, SEEK_END);
      size_t tamanho = ftell(f);
      return tamanho;
   }
} // Inclui o caracter  ?? \r ??


char *Bloco_to_array(FILE *f, FicheiroInf fInf, int num_bloco) {
    size_t tamanhoArray;
    // Verificar se o num_bloco é o ultimo e meter um buffer com um possível espaço irregular em relação aos outros blocos
    if (num_bloco == (fInf -> num_blocos) - 1) tamanhoArray = fInf -> tamanhoUltimoBloco;
    else tamanhoArray = fInf -> tamanhoBloco;

    char *bloco = malloc(tamanhoArray * sizeof(char)); // Criação de um buffer para armazenar os blocos

    fseek(f, (long) ((size_t) num_bloco * (fInf -> tamanhoBloco)), SEEK_SET); // Colocação do apontador de posição no inicio do bloco

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

    size_t num_bloco = 0;

    // Compressão forçada

    if (compressaoForcada) for (num_bloco = 0; num_bloco < fInf -> num_blocos ; num_bloco++)
            compressaoRLEBloco(orig, fInf, rle, num_bloco);

    else {

        // Bloco 1
        compressaoRLEBloco(orig, fInf, rle, num_bloco);

        // Taxa de compressão RLE do primeiro bloco
        double compressao_bloco1 = (double) tamanhoFicheiro(rle) / fInf->tamanhoBloco;
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

    double TaxaCompressao = (double) tamanhoFicheiro(rle) / fInf -> tamanhoTotal;
    printf("Taxa de compressão: %lf\n", TaxaCompressao);

    return TaxaCompressao;
}

void compressaoRLEBloco (FILE *orig, FicheiroInf fInf, FILE *rle, size_t num_bloco) { // Assume-se que está função será chamada com num_bloco sucessivos...
    char *Buffer = Bloco_to_array(orig, fInf, num_bloco);
    int i = 0;
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

        } else { // O símbolo não se repete pelo menos 4 vezes, logo não aplicamos o padrão RLE
            for (; num_repeticoes > 0; num_repeticoes--) fwrite(&simbolo, 1, 1, rle);
        }
    }
    free(Buffer);
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
    // FILE *freq = fopen("ficheiroExemplo.txt.freq","wb"); // Ficheiro freq


    FicheiroInf fInf = NBlocos(orig, TAMANHO_BLOCO, TAMANHO_MINIMO_ULTIMO_BLOCO);
    printf("TamanhoTotal: %ld\nTamanhoBloco: %ld\nTamanhoUltimoBloco: %ld\nNum_Blocos: %d \n", fInf -> tamanhoTotal, fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco, fInf -> num_blocos);

    // CompressãoRLE

    compressaoRLE(orig, fInf, rle, 0);

    // Fechar os ficheiros

    fclose(orig);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Impressão no terminal das informações sobre este módulo
    printf("Tempo de execução: %f segundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC);
    return 0;
}






   
