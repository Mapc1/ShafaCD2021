#include <stdio.h>
#include <string.h>
#include "shafa.h"
#include "modulo-d.h"

void readRLECode(FILE *fpRLE, char *symbol, char *repetitions){
  fread(symbol, 1, 1, fpRLE);
  fread(repetitions, 1, 1, fpRLE);
}

void decodeRLE(FILE *fpRLE, FILE *out){
  int i = 0, bytesWritten = 1;
  char repetitions;
  char symbol, buffer[BUFFSIZE] = "\0";

  while(bytesWritten){
    bytesWritten = fread(&symbol, 1, 1, fpRLE);
    if (bytesWritten){
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
  }
  writeFile(out, buffer, i);
}

void moduleDMain(Options *opts){
  char option = '\0', *foutName, *fin2;
  FILE *fout, *fin1 = fopen(opts->fileIN, "rb");

  switch(opts->opts[3]){
    //case 's': decodeShafa();
    case 'r':
      if(!opts->fileOUT)
        foutName = removeSufix(opts->fileIN, ".rle");
      else
        foutName = opts->fileOUT;
      fout = fopen(foutName, "wb");
      decodeRLE(fin1, fout);
      fclose(fout);
      break;
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n");
  }
  fclose(fin1);
}
