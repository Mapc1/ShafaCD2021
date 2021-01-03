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

void decodeRLE(FILE *fpRLE, FILE *fpOut, FileData *fileData){
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
      if(symbol == 0){
        readRLECode(fpRLE, &symbol, &repetitions);
        counter += 2;
      }
      curSize += repetitions;
      for(; repetitions > 0 && i < BUFFSIZE; i++, repetitions--)
        buffer[i] = symbol;
      if(i >= BUFFSIZE){
        fwrite(buffer, 1, i, fpOut);
        i = 0;
        if(repetitions > 0){
          for(; repetitions > 0; i++, repetitions--)
            buffer[i] = symbol;
        }
      }
    }
    if(block != NULL && counter == block->newSize){
      fprintf(stdout, "Tamanho antes/depois da descodificação RLE (bloco %d): %d/%d\n", block->blockNum + 1, block->newSize, curSize);
      counter = 0;
      curSize = 0;
      block = block->next;
    }
  }
  fwrite(buffer, 1, i, fpOut);
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

BuffQueue *initBuffQueue(){
  BuffQueue *queue = malloc(sizeof(BuffQueue));
  queue->head = NULL;
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

FileData *decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fpOut, FileData *fileData){
  BlockData *block;
  char sectionBuffer[BUFFSIZE];
  int activeThreads = 0, id;
  Args *ocupation[NTHREADS];
  pthread_t threads[NTHREADS];
  Args *args;
  BuffQueue *queue;
  BlockBuff *curr;
  
  for(int i = 0; i < NTHREADS; i++) 
    ocupation[i] = NULL;

  queue = initBuffQueue();

  fseek(fpSF,1,SEEK_SET);
  readSection(fpSF, sectionBuffer);

  block = fileData->first;

  while(block != NULL || activeThreads > 0){
    while(activeThreads < NTHREADS - 1 && block != NULL){
      args = malloc(sizeof(Args));
      args->block = block;

      readSection(fpSF,sectionBuffer);
      args->block->oldSize = strtol(sectionBuffer,NULL,10);

      curr = addQueue(queue, args->block);
      args->blockBuff = curr;

      fread(curr->buffer, 1, args->block->oldSize, fpSF);
      fseek(fpSF, 1, SEEK_CUR);

      id = getFreeThread(ocupation);
      args->blockBuff->threadID = id;
      ocupation[id] = args;
      pthread_create(&threads[id], NULL, decodeSFBlock, args);

      activeThreads++;
      block = block->next;
    }
    if(queue->head != NULL && queue->head->ready){
      id = queue->head->threadID;
      fprintf(stdout, "Tamanho antes/depois da descodificação Shanon-Fano (bloco %d): %d/%d\n",
              ocupation[id]->block->blockNum + 1,
              ocupation[id]->block->oldSize,
              ocupation[id]->block->newSize
              );
      fwrite(queue->head->decoded, 1, queue->head->blockSize, fpOut);
      free(ocupation[id]);
      ocupation[id] = NULL;
      clearHead(queue);
      activeThreads--;
    }
  }
  free(queue);
  return fileData;
}

void moduleDMain(Options *opts){
  FILE *fpOut, *fpSF, *fpCOD, *fpRLE;
  FileData *fileData;
  struct timespec begin, end;
  int elapsed;

  fpOut = fpSF = fpCOD = fpRLE = NULL;

  printf(AUTHORS);
  printf("Inicio da compressão: ");
  data();
  printf("Módulo: d (descodificação dum ficheiro shaf/rle)\n");
  
  clock_gettime(CLOCK_MONOTONIC, &begin);

  switch(opts->optD){
    case 's':
      fpSF = fopen(opts->fileIN, "rb");
      if(!fpSF)
        errorOpenFile(opts->fileIN, READ, fpSF, fpRLE, fpCOD, fpOut);

      fpCOD = getFile(opts->fileCOD, opts->fileIN, "rb", ".cod");
      if(!fpCOD)
        errorOpenFile(opts->fileCOD, READ, fpSF, fpRLE, fpCOD, fpOut);

      fileData = readCOD(fpCOD);
      fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
      if(!fpOut)
        errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpOut);

      decodeShafa(fpSF, fpCOD, fpOut, fileData); 
      break;
    
    case 'r':
      fpRLE = fopen(opts->fileIN, "rb");
      if(!fpRLE)
        errorOpenFile(opts->fileRLE, READ, fpSF, fpRLE, fpCOD, fpOut);

      fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
      if(!fpOut)
        errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpOut);

      decodeRLE(fpRLE, fpOut, NULL);
      break;

    case '\0':
      fpSF = fopen(opts->fileIN, "rb");
      if (!fpSF)
        errorOpenFile(opts->fileIN, READ, fpSF, fpRLE, fpCOD, fpOut);

      fpCOD = getFile(opts->fileCOD, opts->fileIN, "rb", ".cod");
      if(!fpCOD)
        errorOpenFile(opts->fileCOD, READ, fpSF, fpRLE, fpCOD, fpOut);

      fileData = readCOD(fpCOD);
      if(fileData->compress == RLE){
        fpRLE = getFile(opts->fileRLE, opts->fileIN, "wb+", "\0");
        if(!fpRLE)
          errorOpenFile(opts->fileRLE, WRITE, fpSF, fpRLE, fpCOD, fpOut);

        decodeShafa(fpSF, fpCOD, fpRLE, fileData); 

        fpOut = getFile(opts->fileOUT, opts->fileRLE, "wb", "\0");
        if(!fpOut)
          errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpOut);

        fprintf(stdout, "A executar a descodifcação RLE...\n");
        decodeRLE(fpRLE, fpOut, fileData);
      }

      else {
        fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
        if(!fpOut)
          errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpOut);

        decodeShafa(fpSF, fpCOD, fpOut, fileData); 
      }
      break;

    default: fprintf(stderr, "Erro!! Esta opção não existe!\n");
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = ((end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / BILLION) * 1000;

  fprintf(stdout, "Tempo de execução do módulo (milissegundos): %d\n", elapsed);
  fprintf(stdout, "Ficheiro gerado: %s\n", opts->fileOUT);

  if(fpSF)  fclose(fpSF);
  if(fpRLE) fclose(fpRLE);
  if(fpCOD) fclose(fpCOD);
  if(fpOut) fclose(fpOut);
}
