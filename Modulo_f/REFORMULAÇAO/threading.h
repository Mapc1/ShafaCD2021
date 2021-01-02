#ifndef __THREADING__
#define __THREADING__

typedef struct argsThread {
    FILE *orig;
    FicheiroInf fInf;
	unsigned long long numBloco;
	char compressaoForcada;
	unsigned long long *tamanhoRleAcumulado;    
} *ArgsThread;


void* calculoFrequenciasBlocoTHREAD(void* argsThread);
void calculoFrequenciasTHREAD(FILE *orig, FicheiroInf fInf, char compressaoForcada);



#endif 
