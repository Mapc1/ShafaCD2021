#ifndef __FUNCOESAUXILIARES__
#define __FUNCOESAUXILIARES__

typedef unsigned char Byte;

typedef struct Ficheiros {
    FILE *origFreqs;
    FILE *rle;
    FILE *rleFreqs;
} *Fic;

typedef struct ficheiroInf {
    char *nomeFicheiro;
    Fic ficheiros;
    unsigned long long int tamanhoTotal;
    unsigned long int tamanhoBloco;
    long int tamanhoUltimoBloco;
    unsigned long long int numBloco;
} *FicheiroInf;

typedef struct infosBloco {
    Byte *BufferSimbolos;
    Byte *BufferFreqs;
    unsigned long long int tamanhoBufferRle;
    unsigned long long int tamanhoBufferFreqs;
} *InfosBloco;

char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao);

FicheiroInf NBlocos(FILE *f, char *nomeFicheiro, unsigned long int tamanhoBloco);

unsigned long int tamanhoBloco(FicheiroInf fInf, unsigned long long numBloco);

Byte *leituraFicheiro(FILE *f, unsigned long int tamanhoBloco);

void escritaFicheiro(FILE *f, Byte *buffer, unsigned long int tamanho);

void libertarEspacoInfosBloco (InfosBloco infosBloco);

void data();

#endif //MODULO_F_FUNCOESAUXILIARES_H
