#ifndef __SHAFA_H__
#define __SHAFA_H__

#define BUFFSIZE 1024
#define NUMOPTS 4
#define NSIMBOLOS 256
#define CODE_SIZE 2

#define HELP "Shafa - Programa de compressão/descompressão\n\n"\
             "Comandos: shafa -m [t|f|c|d] [-b K|m|M] [-c r]\n"\
             "          -m [t|f|c|d]    Executa o módulo [t|f|c|d]\n"\
             "          -b [K|m|M]      Especifica o tamanho dos blocos para análise\n"\
             "          -d r            Descomprime o ficheiro a partir de um ficheiro rle e outro freq\n"\
             "          -c r            Força a compressão RLE\n"

typedef struct {
  char *fileIN,  //  input file from arguments
       *fileOUT; //  output file needs -o option otherwise fileOUT = '\0' and output is the standard
  char opts[NUMOPTS];  //  Each opt[i] has an option or '\0';
                 //  order:
                 //        opt[0] -> module
                 //        opt[1] -> (-b) [M|m|k]
                 //        opt[2] -> (-c) r
                 //        opt[3] -> (-d) [r|s]
} Options;

// Parses arguments into a Options struct
Options *getOpts(int argc, char *argv[]);

//Writes a file
void writeFile(FILE *out, void *in, int nbytes);

// Remove sufx from the end of src and stores in the returned array
char *removeSufix(char *src, char *sufx);
#endif //__SHAFA_H__
