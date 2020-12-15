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

void initBlock(BlockData *block) {
    block->blockNum = 0;
    block->blockSize = 0;
    block->codes = malloc(sizeof(ABin));
    block->codes->left = NULL;
    block->codes->right = NULL;
    block->next = NULL;
}

ABin *createNode(){
  ABin *ret = malloc(sizeof(ABin));
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}

void getCodes(FILE *fpCOD, BlockData *block){
  ABin **tmp;
  unsigned char bit, gotCode = 0;

  block->codes = createNode();

  for(int symbol = 0; symbol < NSIMBOLOS; symbol++){
    tmp = &(block->codes);
    fread(&bit, 1, 1, fpCOD);
    while(bit != ';' && bit != '@'){
      if(bit == '1'){
          tmp = &((*tmp)->right);
          if(!(*tmp))
            *tmp = createNode();
      }
      else {
          tmp = &((*tmp)->left);
          if(!(*tmp))
              *tmp = createNode();
      }
      fread(&bit, 1, 1, fpCOD);
      gotCode = 1;
    }
    if (gotCode){
        (*tmp)->c = (char) symbol;
        gotCode = 0;
    }
  }
}

BlockData *readCOD(FILE *fpCOD){
  BlockData **tmp, *block;
  char buffer[BUFFSIZE];
  size_t totalBlocks;
  Precomp compression;

  fseek(fpCOD, 1, SEEK_SET);
  readSection(fpCOD, buffer);
  compression = (int) buffer[0];

  readSection(fpCOD, buffer);
  totalBlocks = strtol(buffer, NULL,10);

  tmp = &block;
  for(int i = 0; i < totalBlocks; i++){
      (*tmp) = malloc(sizeof(BlockData));
      initBlock(*tmp);
      (*tmp)->compress = compression;
      (*tmp)->blockNum = i;
      readSection(fpCOD, buffer);
      (*tmp)->blockSize = strtol(buffer, NULL, 10);
      getCodes(fpCOD, *tmp);
      tmp = &((*tmp)->next);
  }
  return block;
}

//recebe um ficheiro do tipoo .cod e retorna 1 se tiver sido ultilizada a compressão rle 0 caso contrário
int itsrle (FILE *f){
    char s[3];
    fgets(s,3,f);
    int r=1;
    if(s[1]=='N') r=0;
  return r;
}

void decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fout){
if(itsrle(fpCOD)==1) { //cria ficheiro do tipo rle
}
  else {//cria ficheiro do tipo original
  }
}

void moduleDMain(Options *opts){
  char *foutName, *fin2;
  FILE *fout, *fin1 = fopen(opts->fileIN, "rb");
  BlockData *blockInfo;

  switch(opts->opts[3]){
    case 's':
      if (!opts->fileOUT)
        foutName = removeSufix(opts->fileIN, ".shafa");
      else foutName = opts->fileOUT;
      fout = fopen(foutName, "wb");
      //decodeShafa();
      blockInfo = readCOD(fin1);
      fseek(fin1, 10, SEEK_SET);
      if(!opts->fileOUT) free(foutName);
      fclose(fout);
      break;
    case 'r':
      if(!opts->fileOUT)
        opts->fileOUT = removeSufix(opts->fileIN, ".rle");
      fout = fopen(foutName, "wb");
      decodeRLE(fin1, fout);
      fclose(fout);
      break;
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n");
  }
  fclose(fin1);
}
