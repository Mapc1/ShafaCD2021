#include <stdio.h>
#include "modulo-d.h"
void printHelp(){
  printf(
         "Shafa - Programa de compressão/descompressão\n\n"
         "Comandos: shafa -m [t|f|c|d] [-b K|m|M] [-c r]\n"
         "          -m [t|f|c|d]    Executa o módulo [t|f|c|d]\n"
         "          -b [K|m|M]      Especifica o tamanho dos blocos para análise\n"
         "          -c r            Força a compressão RLE\n"
       );
}

int main(int argc, char *argv[]){
  if(argc == 1) printHelp();
  return 0;
}
