#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shafa.h"
#include "modulo-d.h"

#ifdef __linux__
  #include <time.h>
  #include <pthread.h>
#endif

#ifdef _WIN32
  #include <windows.h>
#endif

long readSection(FILE *fp, char *str){
  long bytesRead = 0;
  int i;
  for(i = 0; str[i-1] != '@'; i++)
    bytesRead = fread(str+i, 1, 1, fp);
  str[i-1] = '\0';
  
  return bytesRead;
}

long skipsection(FILE *fp){
  int i;
  long bytesRead;
  char c = '\0';
  for(i = 0; c != '@'; i++)
    bytesRead = fread(&c, 1, 1, fp);
  return bytesRead;
}

long readRLECode(FILE *fpRLE, unsigned char *symbol, unsigned char *repetitions){
  long bytesRead = 0;
  bytesRead = fread(symbol, 1, 1, fpRLE);
  bytesRead += fread(repetitions, 1, 1, fpRLE);
  return bytesRead;
}

int readFreqBlockSize(FILE *fpFREQ){
  char buffer[BUFFSIZE];
  int blockSize = 0;

  readSection(fpFREQ, buffer);
  blockSize = strtol(buffer, NULL, 10);
  
  return blockSize;
}

void decodeRLE(FILE *fpRLE, FILE *fpOut, FILE *fpFREQ, FileData *fileData){
  int i = 0, writeFlag = 1, curSize = 0, byteCounter = 0, freqSize = 0, blockTotal = 0, blockNum = 0, bytesRead;
  unsigned char repetitions;
  unsigned char symbol, buffer[BUFFSIZE] = "\0";
  BlockData *block = NULL;
  
  if(fileData != NULL)
    block = fileData->first;
  else{
    fseek(fpFREQ, 3, SEEK_SET);
    readSection(fpFREQ, (char *) buffer);
    blockTotal = strtol((char *) buffer, NULL, 10);
    freqSize = readFreqBlockSize(fpFREQ);
    skipsection(fpFREQ);
  }
  fseek(fpRLE, 0, SEEK_SET);
  while(writeFlag){
    writeFlag = fread(&symbol, 1, 1, fpRLE);
    
    if (writeFlag){
      byteCounter++;
      repetitions = 1;
      if(symbol == 0){
        bytesRead = readRLECode(fpRLE, &symbol, &repetitions);
        if(!bytesRead){
          fprintf(stderr, "Erro!!!\nO bloco %d está corrompido O.o\n", fileData == NULL ? blockNum + 1: block->blockNum + 1);
          exit(2);
        }
        byteCounter += 2;
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
    if(block != NULL && byteCounter == block->newSize){
      fprintf(stdout, "Tamanho antes/depois da descodificação RLE (bloco %d): %d/%d\n", block->blockNum + 1, block->newSize, curSize);
      byteCounter = 0;
      curSize = 0;
      block = block->next;
    }
    else if(blockNum < blockTotal && byteCounter == freqSize){
      fprintf(stdout, "Tamanho antes/depois da descodificação RLE (bloco %d): %d/%d\n", blockNum + 1, byteCounter, curSize);
      byteCounter = 0;
      curSize = 0;
      blockNum++;
      if(blockNum < blockTotal){
        freqSize = readFreqBlockSize(fpFREQ);
        skipsection(fpFREQ);
      }
    }
  }
  fwrite(buffer, 1, i, fpOut);
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
  long bytesRead;
  unsigned char bit, gotCode = 0;

  block->codes = createNode();

  for(int symbol = 0; symbol < NSIMBOLOS; symbol++){
    tmp = &(block->codes);
    bytesRead = fread(&bit, 1, 1, fpCOD);
    if(!bytesRead){
      fprintf(stderr, "Erro!!!\nNão foi possível obter os códigos do ficheiro cod para o bloco %d", block->blockNum + 1);
      exit(2);
    }
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
      bytesRead = fread(&bit, 1, 1, fpCOD);
      if(!bytesRead){
        fprintf(stderr, "Erro!!!\nNão foi possível obter os códigos do ficheiro cod para o bloco %d", block->blockNum + 1);
        exit(2);
      }
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

FileData *decodeShafa(FILE *fpSF, FILE *fpOut, FileData *fileData){
  BlockData *block;
  char sectionBuffer[BUFFSIZE];
  int activeThreads = 0, id;
  Args *ocupation[NTHREADS];
  Args *args;
  BuffQueue *queue;
  BlockBuff *curr;
  long bytesRead;

  #ifdef __linux__
    pthread_t threads[NTHREADS];
  #endif

  for(int i = 0; i < NTHREADS; i++) 
    ocupation[i] = NULL;

  queue = initBuffQueue();

  fseek(fpSF,1,SEEK_SET);
  readSection(fpSF, sectionBuffer);

  block = fileData->first;

  while(block != NULL || activeThreads > 0){
    while(activeThreads < NTHREADS && block != NULL){
      args = malloc(sizeof(Args));
      args->block = block;

      readSection(fpSF,sectionBuffer);
      args->block->oldSize = strtol(sectionBuffer,NULL,10);

      curr = addQueue(queue, args->block);
      args->blockBuff = curr;

      bytesRead = fread(curr->buffer, 1, args->block->oldSize, fpSF);
      if(!bytesRead){
        fprintf(stderr, "Erro!!!\nO ficheiro shafa está corrompido no bloco %d O.o\n", block->blockNum + 1);
        exit(2);
      }
      fseek(fpSF, 1, SEEK_CUR);

      id = getFreeThread(ocupation);
      args->blockBuff->threadID = id;
      ocupation[id] = args;
      #ifdef __linux__
        pthread_create(&threads[id], NULL, decodeSFBlock, args);
      #endif
      #ifdef _WIN32
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) decodeSFBlock, args, 0, NULL);
      #endif
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

void moduleDMain(Options *opts, FileCreated **list){
  FILE *fpOut, *fpSF, *fpCOD, *fpRLE, *fpFREQ;
  FileData *fileData;

  fpOut = fpSF = fpCOD = fpRLE = fpFREQ = NULL;

  printf(AUTHORS);
  printf("Inicio da compressão: ");
  data();
  printf("Módulo: d (descodificação dum ficheiro shaf/rle)\n");
  
  

  switch(opts->optD){
    case 's':
      fpSF = fopen(opts->fileIN, "rb");
      if(!fpSF)
        errorOpenFile(opts->fileIN, READ, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      fpCOD = getFile(opts->fileCOD, opts->fileIN, "rb", ".cod");
      if(!fpCOD)
         errorOpenFile(opts->fileCOD, READ, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      fileData = readCOD(fpCOD);
      fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
      if(!fpOut)
        errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      addFilesCreated(list, opts->fileOUT);
      decodeShafa(fpSF, fpOut, fileData); 
      break;
    
    case 'r':
      fpRLE = fopen(opts->fileIN, "rb");
      if(!fpRLE)
        errorOpenFile(opts->fileIN, READ, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      strcpy(opts->fileFREQ, opts->fileIN);
      strcat(opts->fileFREQ, ".freq");
      fpFREQ = fopen(opts->fileFREQ, "rb");
      if(!fpFREQ)
        errorOpenFile(opts->fileFREQ, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
      if(!fpOut)
        errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      addFilesCreated(list, opts->fileOUT);
      decodeRLE(fpRLE, fpOut, fpFREQ, NULL);
      break;

    case '\0':
      fpSF = fopen(opts->fileIN, "rb");
      if (!fpSF)
        errorOpenFile(opts->fileIN, READ, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      fpCOD = getFile(opts->fileCOD, opts->fileIN, "rb", ".cod");
      if(!fpCOD)
        errorOpenFile(opts->fileCOD, READ, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

      fileData = readCOD(fpCOD);
      if(fileData->compress == RLE){
        fpRLE = getFile(opts->fileRLE, opts->fileIN, "wb+", "\0");
        if(!fpRLE)
          errorOpenFile(opts->fileRLE, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

        addFilesCreated(list, opts->fileRLE);
        decodeShafa(fpSF, fpRLE, fileData); 

        fpOut = getFile(opts->fileOUT, opts->fileRLE, "wb", "\0");
        if(!fpOut)
          errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

        addFilesCreated(list, opts->fileOUT);
        fprintf(stdout, "A executar a descodifcação RLE...\n");
        decodeRLE(fpRLE, fpOut, NULL, fileData);
      }

      else {
        fpOut = getFile(opts->fileOUT, opts->fileIN, "wb", "\0");
        if(!fpOut)
          errorOpenFile(opts->fileOUT, WRITE, fpSF, fpRLE, fpCOD, fpFREQ, fpOut);

        addFilesCreated(list, opts->fileOUT);
        decodeShafa(fpSF, fpOut, fileData); 
      }
      break;

    default: fprintf(stderr, "Erro!! Esta opção não existe!\n");
  }

  if(fpSF)   fclose(fpSF);
  if(fpRLE)  fclose(fpRLE);
  if(fpCOD)  fclose(fpCOD);
  if(fpFREQ) fclose(fpFREQ);
  if(fpOut)  fclose(fpOut);
}