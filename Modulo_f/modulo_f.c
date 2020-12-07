

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


size_t tamMax_Array (size_t tamanhoBloco, size_t tamanhoUltimoBloco) {
	return tamanhoBloco >= tamanhoUltimoBloco ? tamanhoBloco : tamanhoUltimoBloco;
}


char *Bloco_to_array(FILE *f, FicheiroInf fInf, int num_bloco) {
    size_t tamanhoArray;
    // Verificar se o num_bloco é o ultimo e meter um buffer com um possível espaço irregular em relação aos outros blocos
    if ((num_bloco == (fInf -> num_blocos) - 1)) tamanhoArray = fInf -> tamanhoUltimoBloco;
    else tamanhoArray = fInf -> tamanhoBloco;

    char *bloco = malloc(tamanhoArray * sizeof(char)); // Criação de um buffer para armazenar os blocos

    fseek(f, num_bloco * (fInf -> tamanhoBloco), SEEK_SET); // Colocação do apontador de posição no inicio do bloco

    // Escrita do bloco no array;
    int r = fread(bloco, 1, tamanhoArray, f);
    printf("%d\n", r);
    printf("%s\n", bloco);
    return bloco;
}

char compressao(FILE *orig, FicheiroInf fInf, FILE *rle) {
    /* -> Fazer compressão para o bloco 1
     * --> Identificar padrões de 4 ou +
     * --> Identificar 0's
     * --> Aplicar padrão RLE de 3 Bytes para comprimir ou para escrever 0's
     * -> Ver se compensa fazer compressão rle
     */



    // Bloco 1

    // fseek(orig, 0L, SEEK_SET);
    // É preciso o fseek aqui?

    char *Buffer = Bloco_to_array(orig, fInf, 1);
    int i = 0;
    while (i < fInf -> tamanhoBloco) { // Analisar um símbolo de cada vez
        int num_repeticoes = 1;
        char simbolo = Buffer[i];
        while ((i != (fInf -> tamanhoBloco) - 1) && Buffer[i + 1] == simbolo) { // Identificação de padrões;
            num_repeticoes++;
            i++;
        }
        if (num_repeticoes >= 4) { // O símbolo repete-se pelo menos 4 vezes, logo aplicamos o padrão RLE
            char padraoRLE[3] = {0, simbolo, num_repeticoes}; // {0}{simbolo}{número_de_repetições}
            fwrite(padraoRLE, 3, 1, rle);

        } else { // O símbolo não se repete pelo menos 4 vezes, logo não aplicamos o padrão RLE
            for (; num_repeticoes != 0; num_repeticoes--) {
                fwrite(&simbolo, 1, 1, rle);
            }
        }
    }

    // Caso tenha + que 1 bloco:

    // CUIDADO COM O ULTIMO BLOCO QUE PODE TER + SÍMBOLOS DO QUE O RESTO DOS BLOCOS

    // NAO ESQUECER DE DAR FREE AOS BUFFERS DOS BLOCOS!

    // Ver se compensa: TamanhoBloco1 com rle / TamanhoBloco1 <= 0.95

    // Não compensa fazer a compressão rle para o primeiro bloco, logo nao fazemos a compressão rle para o resto dos blocos, temos que apagar o ficheiro .rle
    if (1) return 0;

    // Visto que compensa fazer a compressão rle para o primeiro bloco, fazemos para os restantes.
}

int main() {
    // NAO ESQUECER: GENERALIZAR A MAIN!!!!!!!!!!!!!!!!



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

 	Bloco_to_array(orig,fInf, 4);


    // Fechar os ficheiros

    fclose(orig);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Impressão no terminal das informações sobre este módulo
    printf("Tempo de execução: %f segundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC);
    return 0;
}






   
