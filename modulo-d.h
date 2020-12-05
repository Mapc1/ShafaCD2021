#ifndef __MODULE_D__
#define __MODULE_D__

#define NSIMBOLOS 256

typedef char symbolTable[256];

typedef enum {
  RLE,
  NONE
} Precomp;

void moduleDMain(int argc, char *argv[]);
void decodeRLE(FILE *fpRLE, FILE *out);
#endif //__MODULE_D__
