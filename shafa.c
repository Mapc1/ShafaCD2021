#ifdef linux
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#endif

#ifdef _WIN32
#endif

#include "modulo-d.h"
#include "shafa.h"
#include "Modulo_f/moduloF.h"

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
        case 'b': opts->optB = argv[i][0]; break;
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

void errorOpenFile (char *fileName, Flag flag, FILE *fpSF, FILE *fpRLE, FILE *fpCOD, FILE *fpOut){
  if(flag == READ)
    fprintf(stderr, "ERRO!!!\nNão foi possível abrir o ficheiro %s! -.-\n", fileName);
  else
    fprintf(stderr, "ERRO!!!\nNão foi possível criar o ficheiro %s! -.-\n", fileName);

  if(fpSF)  fclose(fpSF);
  if(fpRLE) fclose(fpRLE);
  if(fpCOD) fclose(fpCOD);
  if(fpOut) fclose(fpOut);
  
  exit(1);
}

FILE *getFile(char *dest, char *fileName, char * mode, char *sufx){
  FILE *fp;

  if(dest[0] == '\0'){
    removeSufix(dest, fileName);
    strcat(dest, sufx);
  }
  fp = fopen(dest, mode);
  return fp;
}

char *removeSufix(char *dest, char *src){
  int i;
  strcpy(dest, src);

  for(i = 0; src[i+1] != '\0'; i++);
  for(; dest[i] != '.'; i--);

  dest[i] = '\0';
  return dest;
}

int main(int argc, char *argv[]){
  Options *opts;

  if(argc == 1) fprintf(stdout, HELP);

  opts = getOpts(argc, argv);
  if(opts->modF) moduleFMain(opts);
  //if(modT)
  if(opts->modD) moduleDMain(opts);
  //if(modC)
  free(opts);
  return 0;
}
