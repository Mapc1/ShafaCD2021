#ifndef __MODULE_D__
#define __MODULE_D__

#include "shafa.h"
#include <stdlib.h>

#define NSIMBOLOS 256
#define CODE_SIZE 2

typedef enum {
  RLE = 'R',
  NONE = 'N'
} Precomp;

typedef struct blockData{
  Precomp compression;
  size_t blockNum, blockSize;
  char symbolMatrix[NSIMBOLOS*8][CODE_SIZE+2];
  struct blockData *next;
} BlockData;

// Main function to handle the d module
void moduleDMain(Options *opts);

// function responsible for decoding .rle files
void decodeRLE(FILE *fpRLE, FILE *out);
#endif //__MODULE_D__
