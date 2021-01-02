/**
 * @file processamento.h
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções necessárias para processar o ficheiro de entrada .
 *
 * A funções deste ficheiro processam o buffer(array de "Bytes", que são
 * unsigned chars) onde foi guardado o ficheiro de entrada.
 * O objetivo passou por criar outros dois buffers, um onde foi guardada
 * a informação que será colocada no ficheiro das frequências e outro
 * onde será guardada a informação que será colocada no ficheiro RLE
 * (se este for gerado).
 *
 */

#ifndef __PROCESSAMENTO__
#define __PROCESSAMENTO__


/**
 * @brief Função que coloca num buffer as informações para depois escrever no ficheiro das frequências do ficheiro original.
 * @param bufferInput Buffer(array) com a informação do ficheiro original.
 * @param tamanhoBlocoInput Tamanho do Bloco(Bloco do ficheiro original) que vai ser analisado.
 * @param BufferFreqs Buffer com as frequências(Neste caso as do ficheiro original).
 */
void freqsOriginal(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs);

/**
 * @brief Função que coloca num buffer as informações para depois escrever no ficheiro das frequências do ficheiro original. Cria também o buffer com a compressão RLE.
 * @param bufferInput Buffer(array) com a informação do ficheiro original.
 * @param tamanhoBlocoInput Tamanho do Bloco(Bloco do ficheiro original) que vai ser analisado.
 * @param BufferFreqs Buffer com as frequências(Neste caso as do ficheiro RLE).
 */
void freqsRle(const Byte *bufferInput, unsigned long int tamanhoBlocoInput, unsigned long long *BufferFreqs, InfosBloco infosBloco, unsigned long long *tamanhoRleAcumulado);

/**
 * @brief Função que transforma o buffer com as frequências num buffer de Bytes(que definimos como unsigned char) para a escrita no ficheiro freq.
 * @param bufferInput Buffer(array) com a informação do ficheiro original.
 * @param tamanhoBlocoInput Tamanho do Bloco(Bloco do ficheiro original) que vai ser analisado. 
 * @param BufferFreqs Buffer com as frequências, ou seja, o buffer que é transformado.
 */
void freqsParaEscrita(unsigned long long *BufferFreqs, unsigned long long numBloco, FicheiroInf fInf, InfosBloco infosBloco);

/**
 * @brief Função que cria a struct, essencial para a escrita nos ficheiros.
 * @param bufferInput Buffer(array) com a informação do ficheiro original.
 * @param fInf Struct com as informações sobre o ficheiro original.
 * @param numBloco Número do bloco que se vai analisar(Na verdade é o número do bloco - 1).
 * @param tamanhoRleAcumulado Apontador para o tamanho do buffer do RLE até ao momento.
 * @return Struct com os buffers para a escrita nos ficheiros, bem como o tamanho de cada um deles.
 */
InfosBloco processamento(Byte *bufferInput, FicheiroInf fInf, unsigned long long numBloco, unsigned long long *tamanhoRleAcumulado);


#endif //__MODULO_F__
