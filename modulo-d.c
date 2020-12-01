#include <stdio.h>
#include <string.h>
#include "shafa.h"

void moduleDMain(int argc, char *argv[]){
  char option = '\0';
  if(argc > 4 && strcmp(argv[4], "-d") == 0)
    option = argv[5];

  switch(option){
    //case 's': decodeShafa();
    //case 'r': decodeRLE();
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n")
  }
}
