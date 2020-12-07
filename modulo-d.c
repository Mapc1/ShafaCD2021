#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void readSection(FILE *fp, char *str){
  int i;
  for(i = 0; str[i-1] != '@'; i++)
    fread(str+i, 1, 1, fp);
  str[i-1] = '\0';
}

void getCodesAux(FILE *fpCOD, BlockData *block, char cur){
  char symbol, offset, index;
  size_t pos = ftell(fpCOD);

  for(offset = 0; offset < 8; offset++){
    fread(&symbol, 1, 1, fpCOD);
    if(symbol == ';') break;
    for(index = offset; symbol != ';'; index++){
      if(index < 8)
        block->symbolMatrix[offset*NSIMBOLOS + cur][0] = (block->symbolMatrix[offset*NSIMBOLOS + cur][0] | ((symbol - '0') & (1 << (8 - index))));
      else
        block->symbolMatrix[offset*NSIMBOLOS + cur][1] = (block->symbolMatrix[offset*NSIMBOLOS + cur][1] | ((symbol - '0') & (1 << (index - 8))));
      fread(&symbol, 1, 1, fpCOD);
    }
    fseek(fpCOD, pos, SEEK_SET);
  }
}

void getCodes(FILE *fpCOD, BlockData *block){
  for(int i = 0; i < NSIMBOLOS; i++){
    getCodesAux(fpCOD, block, i);
  }
}

BlockData *readCOD(FILE *fpCOD){
  BlockData *tmp, *block = malloc(sizeof(BlockData));
  char buffer[BUFFSIZE];
  size_t totalBlocks;
  fread(&stdout, 1, 1, fpCOD);
  readSection(fpCOD, buffer);
  block->compression = buffer[0];

  readSection(fpCOD, buffer);
  totalBlocks = atoi(buffer);

  tmp = block;
  for(int i = 0; i < totalBlocks; i++){
    block->blockNum = i;
    readSection(fpCOD, buffer);
    block->blockSize = atoi(buffer);
    getCodes(fpCOD, tmp);
    tmp = tmp->next;
  }
  return block;
}

void decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fout){

}

void moduleDMain(Options *opts){
  char *foutName, *fin2;
  FILE *fout, *fin1 = fopen(opts->fileIN, "rb");

  switch(opts->opts[3]){
    case 's':
      if (!opts->fileOUT)
        foutName = removeSufix(opts->fileIN, ".shafa");
      else foutName = opts->fileOUT;
      fout = fopen(foutName, "wb");
      //decodeShafa();
      readCOD(fin1);
      if(!opts->fileOUT) free(foutName);
      fclose(fout);
      break;
    case 'r':
      if(!opts->fileOUT)
        foutName = removeSufix(opts->fileIN, ".rle");
      else
        foutName = opts->fileOUT;
      fout = fopen(foutName, "wb");
      decodeRLE(fin1, fout);
      if (!opts->fileOUT) free(foutName);
      fclose(fout);
      break;
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n");
  }
  fclose(fin1);
}
