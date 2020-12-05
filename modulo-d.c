#include <stdio.h>
#include <string.h>
#include "shafa.h"
#include "modulo-d.h"

void readRLECode(FILE *fpRLE, char *symbol, char *repetitions){
  fread(symbol, 1, 1, fpRLE);
  fread(repetitions, 1, 1, fpRLE);
}

void decodeRLE(FILE *fpRLE, FILE *out){
  int i = 0;
  char symbol, repetitions, buffer[BUFFSIZE] = "\0";

  while(symbol != EOF){
    symbol = fgetc(fpRLE);
    repetitions = 1;
    if(symbol == 0)
      readRLECode(fpRLE, &symbol, &repetitions);
    for(; repetitions > 0 && i < BUFFSIZE; i++, repetitions--)
      buffer[i] = symbol;
    if(i >= BUFFSIZE){
      writeFile(out, buffer, i);
      i = 0;
      if(repetitions > 0){
        for(; repetitions > 0 || i > BUFFSIZE; i++, repetitions--)
          buffer[i] = symbol;
      }
    }
  }
  writeFile(out, buffer, i);
}

void moduleDMain(int argc, char *argv[]){
  char option = '\0';
  FILE *fp = fopen("yay", "w");
  FILE *fin= fopen("files/aaa.txt.rle", "r");
  if(argc > 4 && strcmp(argv[4], "-d") == 0)
    option = argv[5][0];

  switch(option){
    //case 's': decodeShafa();
    case 'r': decodeRLE(fin, fp); break;
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n");
  }
}
