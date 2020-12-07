

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

size_t tamanhoFicheiro (FILE *f){
   if (!f) return 0;
   else {
      fseek(f, 0L, SEEK_END);
      size_t tamanho = ftell(f);
      return tamanho;
   }
} // Inclui o caracter  ?? \r ??


size_t tamMax_Array (size_t tamanhoBloco, size_t tamanhoUltimoBloco){
	return tamanhoBloco >= tamanhoUltimoBloco ? tamanhoBloco : tamanhoUltimoBloco;
}


void Bloco1_to_array(FILE *f, FicheiroInf fInf){

    int i;
    size_t tamanhoArray = tamMax_Array(fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco); //o buffer terá sempre o tamanho ideal; certifica-se que há espaço no mesmo, quando o Último Bloco é o maior
    char bloco1[tamanhoArray]; // Bloco1 é o buffer
    fseek(f, 0, SEEK_SET);

    for (size_t i = 0; (fread(bloco1, NumBytesLidos, tamanhoArray, f)); ++i) {
        printf("%c\n", bloco1[i]);
    }

}

int main() {
    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Abertura dos ficheiros

    FILE *orig;
    orig = fopen("../aaa.txt","rb"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!\n"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }
    // FILE *rle = fopen("ficheiroExemplo.txt.rle","wb"); // Ficheiro rle
    // FILE *freq = fopen("ficheiroExemplo.txt.freq","wb"); // Ficheiro freq


    FicheiroInf fInf = NBlocos(orig, 512, 1024);
    printf("TamanhoTotal: %ld\nTamanhoBloco: %ld\nTamanhoUltimoBloco: %ld\nNum_Blocos: %d \n", fInf -> tamanhoTotal, fInf -> tamanhoBloco, fInf -> tamanhoUltimoBloco, fInf -> num_blocos);

 	Bloco1_to_array(orig,fInf);


    // Fechar os ficheiros

    fclose(orig);

    // Fim da contagem do tempo de execução
    clock_t fim = clock();

    // Impressão no terminal das informações sobre este módulo
    printf("Tempo de execução: %f segundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC);
    return 0;
}






   
