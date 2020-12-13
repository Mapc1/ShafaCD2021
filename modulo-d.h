#ifndef __MODULE_D__
#define __MODULE_D__

#include "shafa.h"
#include <stdlib.h>

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
  size_t blockNum, blockSize;
  //char symbolMatrix[NSIMBOLOS*8][CODE_SIZE+2];
  ABin *codes;
  struct blockData *next;
} BlockData;

// Main function to handle the d module
void moduleDMain(Options *opts);

// function responsible for decoding .rle files
void decodeRLE(FILE *fpRLE, FILE *out);
#endif //__MODULE_D__
