#ifndef __SHAFA_H__
#define __SHAFA_H__

#define BUFFSIZE 1024
#define NSIMBOLOS 256
#define NTHREADS 8
#define BILLION  1000000000.0

#define HELP "Shafa - Programa de compressão/descompressão\n\n"\
             "Comandos: shafa -m [t|f|c|d] [-b K|m|M] [-c r]\n"\
             "          -m [t|f|c|d]    Executa o módulo [t|f|c|d]\n"\
             "          -b [K|m|M]      Especifica o tamanho dos blocos para análise\n"\
             "          -d r            Descomprime o ficheiro a partir de um ficheiro rle e outro freq\n"\
             "          -c r            Força a compressão RLE\n"

typedef struct {
  char *fileIN,               //  input file from arguments
       fileOUT[BUFFSIZE];              //  output file needs -o option otherwise fileOUT = '\0' and output is the standard
  int modT, modD, modF, modC; //  Modules
  int optB, optC, optD;       //  The various possible options
} Options;

// Prints today's date
void data();

//  Parses arguments into a Options struct
Options *getOpts(int argc, char *argv[]);

//  Copies the string stored in src to dest and removes sufx from the end of dest
//  This function returns the address to dest just like strcpy
char *removeSufix(char *dest, char *src, char *sufx);
#endif //__SHAFA_H__
