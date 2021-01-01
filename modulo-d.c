#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "shafa.h"
#include "modulo-d.h"

void readRLECode(FILE *fpRLE, unsigned char *symbol, unsigned char *repetitions){
  fread(symbol, 1, 1, fpRLE);
  fread(repetitions, 1, 1, fpRLE);
}

void decodeRLE(FILE *fpRLE, FILE *fout, FileData *fileData){
  int i = 0, writeFlag = 1, curSize = 0, counter = 0;
  unsigned char repetitions;
  unsigned char symbol, buffer[BUFFSIZE] = "\0";
  BlockData *block;
  
  if(fileData != NULL)
    block = fileData->first;

  fseek(fpRLE, 0, SEEK_SET);
  while(writeFlag){
    writeFlag = fread(&symbol, 1, 1, fpRLE);
    
    if (writeFlag){
      counter++;
      repetitions = 1;
      if(symbol == 0)
        readRLECode(fpRLE, &symbol, &repetitions);
      curSize += repetitions;
      for(; repetitions > 0 && i < BUFFSIZE; i++, repetitions--)
        buffer[i] = symbol;
      if(i >= BUFFSIZE){
        fwrite(buffer, 1, i, fout);
        i = 0;
        if(repetitions > 0){
          for(; repetitions > 0; i++, repetitions--)
            buffer[i] = symbol;
        }
      }
    }
    if(block != NULL && counter == block->newSize){
      fprintf(stdout, "Tamanho antes/depois a descodificação RLE (bloco %d): %d/%d\n", block->blockNum + 1, block->newSize, curSize);
      counter = 0;
      curSize = 0;
      block = block->next;
    }
  }
  fwrite(buffer, 1, i, fout);
}

int readSection(FILE *fp, char *str){
  int i;
  for(i = 0; str[i-1] != '@'; i++)
    fread(str+i, 1, 1, fp);
  str[i-1] = '\0';
  
  return i;
}

void initFileData(FileData *data){
  data->compress = NONE;
  data->first = NULL;
}

void initBlock(BlockData *block) {
    block->blockNum = 0;
    block->oldSize = 0;
    block->newSize = 0;
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

FileData *readCOD(FILE *fpCOD){
  FileData *ret;
  BlockData **block;
  char buffer[BUFFSIZE];
  int totalBlocks;

  ret = malloc(sizeof(FileData));

  fseek(fpCOD, 1, SEEK_SET);
  readSection(fpCOD, buffer);
  ret->compress = (int) buffer[0];

  readSection(fpCOD, buffer);
  totalBlocks = strtol(buffer, NULL,10);
  fprintf(stdout, "Número de blocos: %d\n", totalBlocks);

  block = &(ret->first);
  for(int i = 0; i < totalBlocks; i++){
      (*block) = malloc(sizeof(BlockData));
      initBlock(*block);
      (*block)->blockNum = i;
      readSection(fpCOD, buffer);
      (*block)->newSize = strtol(buffer, NULL, 10);
      getCodes(fpCOD, *block);
      block = &((*block)->next);
  }
  return ret;
}

void freeBlockData (BlockData *block){
  BlockData *tmp;
  while(!block){
    tmp = block->next;
    free(block);
    block = tmp;
  }
}

void *decodeSFBlock(void *arg){
  Args *args = (Args *) arg;
  ABin *tmp;
  unsigned char byte;
  int decodedBytes = 0, a = 0;

  tmp = args->block->codes;

  for(int o = 0; o < args->block->oldSize; o++) {
    byte = args->blockBuff->buffer[o];
    for(int index = 0; index < 8 && (decodedBytes < args->block->newSize); index++) {
      if((byte & (1 << (7-index)))) {
        tmp=tmp->right;
      }
      else {
        tmp=tmp->left;
      }
      if(tmp->left==NULL && tmp->right==NULL) {
        args->blockBuff->decoded[a] = tmp->c;
        tmp = args->block->codes;
        decodedBytes++;
        a++;
      }
    }
  }
  args->blockBuff->ready = 1;
  return arg;
}

BuffQueue *initBuffQueue(Args **ocupation, int *activeThreads, int *stopFlag, FILE *fout){
  BuffQueue *queue = malloc(sizeof(BuffQueue));
  queue->ocupation = ocupation;
  queue->fout = fout;
  queue->head = NULL;
  queue->activeThreads = activeThreads;
  queue->stopFlag = stopFlag;
  return queue;
}

BlockBuff *addQueue(BuffQueue *queue, BlockData *block) {
  BlockBuff **tmp, *blockBuff = malloc(sizeof(BlockBuff));

  blockBuff->buffer = malloc(block->oldSize);
  blockBuff->decoded = malloc(block->newSize);
  blockBuff->blockNum = block->blockNum;
  blockBuff->blockSize = block->newSize;
  blockBuff->ready = 0;
  blockBuff->next = NULL;

  tmp = &(queue->head);
  while(*tmp != NULL)
    tmp = &((*tmp)->next);

  *tmp = blockBuff;

  return blockBuff;
}

void clearHead(BuffQueue *queue){
  BlockBuff *tmp = queue->head;

  queue->head = queue->head->next;
  free(tmp->decoded);
  free(tmp);
}

int getFreeThread(Args **ocupation){
  int id;
  for(id = 0; id < NTHREADS; id++)
    if(ocupation[id] == NULL) return id;
  
  return -1;
}

void *writeBlock(void *voidQueue){
  BuffQueue *queue = (BuffQueue *) voidQueue;
  while(1){
      if(queue->head != NULL && queue->head->ready){
        fprintf(stdout, "Tamanho antes/depois da descodificação Shanon-Fano (bloco %d): %d/%d\n",
                queue->ocupation[queue->head->threadID]->block->blockNum + 1,
                queue->ocupation[queue->head->threadID]->block->oldSize,
                queue->ocupation[queue->head->threadID]->block->newSize
                );
        fwrite(queue->head->decoded, 1, queue->head->blockSize, queue->fout);
        free(queue->ocupation[queue->head->threadID]);
        queue->ocupation[queue->head->threadID] = NULL;
        clearHead(queue);
        *(queue->activeThreads) -= 1;
      }
    }
  return queue;
}

FileData *decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fout){
  FileData *fileData;
  BlockData *block;
  char sectionBuffer[BUFFSIZE];
  int activeThreads = 0, stopFlag = 0;
  Args *ocupation[NTHREADS];
  pthread_t threads[NTHREADS];
  Args *args;
  BuffQueue *queue;
  BlockBuff *curr;
  
  for(int i = 0; i < NTHREADS; i++) 
    ocupation[i] = NULL;

  fileData = readCOD(fpCOD);
  queue = initBuffQueue(ocupation, &activeThreads, &stopFlag, fout);

  fseek(fpSF,1,SEEK_SET);
  readSection(fpSF, sectionBuffer);

  block = fileData->first;
  pthread_create(&threads[0], NULL, writeBlock, queue);
  activeThreads++;

  while(block != NULL || activeThreads > 1){
    while(activeThreads < NTHREADS - 1 && block != NULL){
      args = malloc(sizeof(Args));
      args->block = block;

      readSection(fpSF,sectionBuffer);
      args->block->oldSize = strtol(sectionBuffer,NULL,10);

      curr = addQueue(queue, args->block);
      args->blockBuff = curr;

      fread(curr->buffer, 1, args->block->oldSize, fpSF);
      fseek(fpSF, 1, SEEK_CUR);

      args->blockBuff->threadID = getFreeThread(ocupation);
      ocupation[args->blockBuff->threadID] = args;
      pthread_create(&threads[args->blockBuff->threadID], NULL, decodeSFBlock, args);

      activeThreads++;
      block = block->next;
    }
  }
  stopFlag = 1;
  free(queue);
  return fileData;
}

void moduleDMain(Options *opts){
  char codNAME[BUFFSIZE], decodedSF[BUFFSIZE];
  FILE *fout, *fpDecodedSF, *fin, *fin2;
  FileData *fileData;
  struct timespec begin, end;
  double elapsed;

  fin = fopen(opts->fileIN, "rb");

  printf(AUTHORS);
  printf("Inicio da compressão: ");
  data();
  printf("Módulo: d (descodificação dum ficheiro shaf/rle)\n");
  
  clock_gettime(CLOCK_MONOTONIC, &begin);

  switch(opts->optD){
    case 's':
      if (opts->fileOUT[0] == '\0')
        removeSufix(opts->fileOUT, opts->fileIN, ".shaf"); 
      fout = fopen(opts->fileOUT, "wb"); 
      removeSufix(codNAME, opts->fileIN, ".shaf");
      strcat(codNAME, ".cod");
      fin2 = fopen(codNAME,"rb");
      decodeShafa(fin, fin2, fout); 
      fclose(fout);
      break;
    
    case 'r':
      if(opts->fileOUT[0] == '\0')
        removeSufix(opts->fileOUT, opts->fileIN, ".rle");
      fout = fopen(opts->fileOUT, "wb");
      if(!opts->optB) 
      decodeRLE(fin, fout, NULL);
      fclose(fout);
      break;

    case '\0':      
      removeSufix(codNAME, opts->fileIN, ".shaf");
      strcat(codNAME, ".cod");
      fin2 = fopen(codNAME,"rb");
      removeSufix(decodedSF, codNAME, ".cod");
      fpDecodedSF = fopen(decodedSF, "wb+");
      fileData = decodeShafa(fin, fin2, fpDecodedSF); 
      if(fileData->compress == RLE){
        fprintf(stdout, "A executar a descodifcação RLE...\n");
        fseek(fpDecodedSF, 0, SEEK_SET);
        if(opts->fileOUT[0] == '\0')
          removeSufix(opts->fileOUT, decodedSF, ".rle");
        fout = fopen(opts->fileOUT, "wb");
        decodeRLE(fpDecodedSF, fout, fileData);
        fclose(fout);
      }
      else if(opts->fileOUT[0] != '\0')
        rename(decodedSF, opts->fileOUT);
      else strcpy(opts->fileOUT, decodedSF);
      break;

    default: fprintf(stderr, "Erro!! Esta opção não existe!\n");
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_nsec - begin.tv_nsec) / BILLION);
  
  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %.3lf\n", elapsed);
  fprintf(stdout, "Ficheiro gerado: %s\n", opts->fileOUT);

  fclose(fin);
}
