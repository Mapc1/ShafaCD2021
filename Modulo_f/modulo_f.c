//
// Created by goncalo on 06/12/20.
//

#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long int tamanhoFicheiro (FILE *f){
   if (!f) return 0;
   else {
      fseek(f, 0L, SEEK_END);
      int tamanho = ftell(f); 
      return tamanho;
   }
} //Inclui o caracter  ?? \r ??

int main() {
    // Início da contagem do tempo de execução
    clock_t inicio = clock();

    // Abertura dos ficheiros

    FILE *orig;
    orig = fopen("aaa.txt","r+"); // Ficheiro original

    if (!orig) {
        printf("Erro ao abrir o ficheiro!"); // Caso haja erro na leitura do ficheiro original, o programa termina
        exit(1);
    }
    // FILE *rle = fopen("ficheiroExemplo.txt.rle","w"); // Ficheiro rle
    // FILE *freq = fopen("ficheiroExemplo.txt.freq","w"); // Ficheiro freq

    printf("O tamanho do ficheiro é %ld bytes\n", tamanhoFicheiro(orig));


    // Fechar os ficheiros

    fclose(orig);

    // Fim da contagem do tempo de execução
//    clock_t fim = clock();

    // Impressão no terminal das informações sobre este módulo
 //   printf("Tempo de execução: %f segundos", ((double)(fim - inicio)) / CLOCKS_PER_SEC);
//    return 0;
}


/*int leituraF(FILE *f) {
    while (fread())
    fread();
    return ;
}    */



   