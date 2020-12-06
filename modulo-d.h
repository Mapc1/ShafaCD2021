#ifndef __MODULE_D__
#define __MODULE_D__

#include "shafa.h"

#define NSIMBOLOS 256

typedef char symbolTable[256];

typedef enum {
  RLE,
  NONE
} Precomp;

void moduleDMain(Options *opts);
void decodeRLE(FILE *fpRLE, FILE *out);
#endif //__MODULE_D__
