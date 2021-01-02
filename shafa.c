#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "modulo-d.h"
#include "shafa.h"

int initOpts(Options *opts){
  if(!opts) return 0;

  opts->fileIN = NULL;
  opts->fileOUT[0] = '\0';
  opts->modT = 0;
  opts->modD = 0;
  opts->modF = 0;
  opts->modC = 0;
  opts->optB = 0;
  opts->optD = 0;
  opts->optC = 0;
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
        case 'm': 
          switch(argv[i][0]){
            case 't': opts->modT = 1; break;
            case 'd': opts->modD = 1; break;
            case 'f': opts->modF = 1; break;
            case 'c': opts->modC = 1; break;
          }; break;
        case 'b': opts->optB = strtol(argv[i], NULL, 10); break;
        case 'c': opts->optC = argv[i][0]; break;
        case 'd': opts->optD = argv[i][0]; break;
        case 'o': strcpy(opts->fileOUT, argv[i]); break;
        default : fprintf(stdout, "ERRO!!\nA opção '-%c' não existe!\n", argv[i-1][1]);
      }
    }
    else opts->fileIN = argv[i];
    i++;
  }
  return opts;
}

void data() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);    
    printf("%02d-%02d-%d  %02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_hour);
}

char *removeSufix(char *dest, char *src, char *sufx){
  int i, j;
  strcpy(dest, src);

  for(i = 0; src[i+1] != '\0'; i++);
  for(j = 0; sufx[j] != '\0'; j++);
  for(; i > 0 && j > 0; i--, j--)
      dest[i] = '\0';

  return dest;
}

int main(int argc, char *argv[]){
  Options *opts;

  if(argc == 1) fprintf(stdout, HELP);

  opts = getOpts(argc, argv);
  //if(modT)
  if(opts->modD) moduleDMain(opts);
  //if(modF)
  //if(modC)
  free(opts);
  return 0;
}
