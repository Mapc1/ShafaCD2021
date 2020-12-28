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
        fwrite(buffer, 1, i, out);
        i = 0;
        if(repetitions > 0){
          for(; repetitions > 0 || i > BUFFSIZE; i++, repetitions--)
            buffer[i] = symbol;
        }
      }
    }
  }
  fwrite(buffer, 1, i, out);
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

void freeBlockData (BlockData *block){
  BlockData *tmp;
  while(!block){
    tmp = block->next;
    free(block);
    block = tmp;
  }
}

Precomp decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fout){
  BlockData *aux, *block = readCOD(fpCOD);
  Precomp compression = block->compress;
  unsigned char byte;
  char sectionbuffer[BUFFSIZE],buffer[BUFFSIZE];
  ABin *tmp;
  tmp = block->codes;
  int tam,nblock, decodedBytes = 0, a = 0;
  
  fseek(fpSF,1,SEEK_SET);
  readSection(fpSF,sectionbuffer);
  nblock=strtoll(sectionbuffer,NULL,10);
 
  aux = block;

  for(int i=0;i<nblock;i++) {
    readSection(fpSF,sectionbuffer);
    tam=strtoll(sectionbuffer,NULL,10);
    for(int o = 0; o < tam; o++) {
      fread(&byte,1,1,fpSF);
      for(int index=0; index<8 && (decodedBytes < aux->blockSize); index++) {
        if((byte & (1 << (7-index)))) {
          tmp=tmp->right;
        }
        else {
          tmp=tmp->left;
        }
        if(tmp->left==NULL && tmp->right==NULL) {
          buffer[a]=tmp->c;
          tmp=aux->codes;
          decodedBytes++;
          a++;
          if(a==BUFFSIZE) {
            fwrite(buffer, 1, a,fout);
            a = 0;
          }
        }
      }
    }
    decodedBytes = 0;
    aux = aux->next;
    fseek(fpSF, 1, SEEK_CUR);
  }

  if(a > 0) fwrite(buffer, 1, a, fout);
  free(block);
  return compression;
}

void moduleDMain(Options *opts){
  char *codNAME, *rleNAME;
  FILE *fout, *fpRLE, *fin = fopen(opts->fileIN, "rb"), *fin2;
  Precomp compression;

  switch(opts->optD){
    case 's':
      if (!opts->fileOUT)
        opts->fileOUT = removeSufix(opts->fileIN, ".shaf"); 
      fout = fopen(opts->fileOUT, "wb"); 
      codNAME = removeSufix(opts->fileIN, ".shaf");
      strcat(codNAME, ".cod");
      fin2 = fopen(codNAME,"rb");
      decodeShafa(fin, fin2, fout); 
      fclose(fout);
      if(opts->fileOUT) free(opts->fileOUT);
      break;
    
    case 'r':
      if(!opts->fileOUT)
        opts->fileOUT = removeSufix(opts->fileIN, ".rle");
      fout = fopen(opts->fileOUT, "wb");
      decodeRLE(fin, fout);
      fclose(fout);
      break;

    case '\0':      
      codNAME = removeSufix(opts->fileIN, ".shaf");
      strcat(codNAME, ".cod");
      fin2 = fopen(codNAME,"rb");
      rleNAME = removeSufix(codNAME, ".cod");
      fpRLE = fopen(rleNAME, "wb+");
      compression = decodeShafa(fin, fin2, fpRLE); 
      if(compression == RLE){
        fseek(fpRLE, 0, SEEK_SET);
        if(!opts->fileOUT)
          opts->fileOUT = removeSufix(rleNAME, ".rle");
        fout = fopen(opts->fileOUT, "wb");
        decodeRLE(fpRLE, fout);
        fclose(fout);
      }
      break;
    default: fprintf(stderr, "Erro!! Esta opção não existe!\n");
  }
  fclose(fin);
}
