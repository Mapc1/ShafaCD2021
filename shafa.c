#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "modulo-d.h"
#include "shafa.h"

int initOpts(Options *opts){
  if(!opts) return 0;

  opts->fileIN = NULL;
  opts->fileOUT = NULL;
  strcpy(opts->opts, "\0\0\0\0");
  return 1;
}

Options *getOpts(int argc, char *argv[]){
  int i = 1;
  Options *opts = malloc(sizeof(Options));
  initOpts(opts);

  while(i < argc){
    if(argv[i][0] == '-'){
      i++;
      switch(argv[i-1][1]){
        case 'm': opts->opts[0] = argv[i][0]; break;
        case 'b': opts->opts[1] = argv[i][0]; break;
        case 'c': opts->opts[2] = argv[i][0]; break;
        case 'd': opts->opts[3] = argv[i][0]; break;
        case 'o': opts->fileOUT = argv[i]; break;
      }
    }
    else opts->fileIN = argv[i];
    i++;
  }
  return opts;
}

char *removeSufix(char *src, char *sufx){
  int i, j;
  char *dest;
  strcpy(dest, src);

  for(i = 0; src[i] != '\0'; i++);
  for(j = 0; sufx[j] != '\0'; j++);
  for(; i > 0 && j > 0; i--, j--)
      dest[i] = '\0';

  return dest;
}

void writeFile(FILE *out, void *in, int nbytes){
  fwrite(in, 1, nbytes, out);
}

int main(int argc, char *argv[]){
  Options *opts;

  if(argc == 1) fprintf(stdout, HELP);

  opts = getOpts(argc, argv);
  switch(opts->opts[0]){
//    case 'f':
//    case 't':
//    case 'c':
    case 'd': moduleDMain(opts);
  }
  return 0;
}
