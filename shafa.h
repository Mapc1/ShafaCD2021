#ifndef __SHAFA_H__
#define __SHAFA_H__

#define BUFFSIZE 1024

#define HELP "Shafa - Programa de compressão/descompressão\n\n"\
             "Comandos: shafa -m [t|f|c|d] [-b K|m|M] [-c r]\n"\
             "          -m [t|f|c|d]    Executa o módulo [t|f|c|d]\n"\
             "          -b [K|m|M]      Especifica o tamanho dos blocos para análise\n"\
             "          -d r            Descomprime o ficheiro a partir de um ficheiro rle e outro freq\n"\
             "          -c r            Força a compressão RLE\n"

void writeFile(FILE *out, void *in, int nbytes);

#endif //__SHAFA_H__
