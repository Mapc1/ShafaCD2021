#include <stdio.h>
#include "modulo-d.h"
#include "shafa.h"

void writeFile(FILE *out, void *in, int nbytes){
  fwrite(in, 1, nbytes, out);
}

int main(int argc, char *argv[]){
  if(argc == 1) fprintf(stdout, HELP);
  return 0;
}
