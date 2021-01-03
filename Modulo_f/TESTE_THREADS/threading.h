#ifndef __THREADING__
#define __THREADING__

typedef struct argsThread {
    FILE *orig;
    FicheiroInf fInf;
	unsigned long long numBloco;
	unsigned long long *tamanhoRleAcumulado; //se for NULL nao é feito RLE 
} *ArgsThread;


void* calculoFrequenciasBlocoTHREAD(void* argsThread);
void calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada);



#endif 
