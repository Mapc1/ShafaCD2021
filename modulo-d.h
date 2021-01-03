#ifndef __MODULE_D__
#define __MODULE_D__
#include "shafa.h"
#include <stdlib.h>
#include <stdio.h>

#define AUTHORS "Autores: Marco Costa: A93283  Daniel Azevedo: A93324 \n\n"

typedef enum {
  RLE = 'R',
  NONE = 'N'
} Precomp;

typedef struct abin {
  unsigned char c;
  struct abin *left;
  struct abin *right;
} ABin;

typedef struct blockData{
  int blockNum, oldSize, newSize;
  ABin *codes;
  struct blockData *next;
} BlockData;

typedef struct {
  Precomp compress;
  BlockData *first;
} FileData;

typedef struct blockBuff {
  char *buffer, *decoded;
  int blockNum, blockSize, ready, threadID;
  struct blockBuff *next;
} BlockBuff;

typedef struct buffQueue {
  BlockBuff *head;
} BuffQueue;

typedef struct args{
  BlockBuff *blockBuff;
  BlockData *block;
  struct args **ocupation;
} Args;

// Main function to handle the d module
void moduleDMain(Options *opts);

// function responsible for decoding .rle files
void decodeRLE(FILE *fpRLE, FILE *out, FILE *fpFREQ, FileData *fileData);
#endif //__MODULE_D__
