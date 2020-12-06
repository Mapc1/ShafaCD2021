#ifndef __MODULE_D__
#define __MODULE_D__

#include "shafa.h"

#define NSIMBOLOS 256

typedef char symbolTable[256];

typedef enum {
  RLE,
  NONE
} Precomp;

// Main function to handle the d module
void moduleDMain(Options *opts);

// function responsible for decoding .rle files
void decodeRLE(FILE *fpRLE, FILE *out);
#endif //__MODULE_D__
