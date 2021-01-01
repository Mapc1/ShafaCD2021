#ifndef __PROCESSAMENTO__
#define __PROCESSAMENTO__

void freqsOriginal(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs);

void freqsRle(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs, InfosBloco infosBloco, unsigned long long *tamanhoRleAcumulado);

void freqsParaEscrita(unsigned long long *BufferFreqs, unsigned long long numBloco, FicheiroInf fInf, InfosBloco infosBloco);

InfosBloco processamento(Byte *bufferInput, FicheiroInf fInf, unsigned long long numBloco, unsigned long long *tamanhoRleAcumulado);


#endif //__MODULO_F__
