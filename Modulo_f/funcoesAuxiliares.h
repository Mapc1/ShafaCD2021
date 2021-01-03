/**
 * @file funcoesAuxiliares.h
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções e structs auxiliares.
 *
 * A funções deste ficheiro são funções que não interferem com o
 * processamento da informação dos ficheiros, sendo funções
 * que ajudam na correta execução do programa.
 * Neste grupo de funções, temos, por exemplo, funções que gerem a 
 * forma como a informação é apresentada no terminal.
 * Temos também aqui definidas as structs onde guardamos informações
 * essenciais para a execução do Módulo.
 *
 */

#ifndef __FUNCOESAUXILIARES__
#define __FUNCOESAUXILIARES__

#include "../shafa.h"

/**
 * @brief Definido somente para não ter de escrever inúmeras vezes "unsigned char"
 *
 */
typedef unsigned char Byte;

/**
 * @brief Struct com os ficheiros que são/podem ser gerados pelo Módulo f.
 *
 */
typedef struct Ficheiros {
    FILE *origFreqs; /**< Ficheiro com as frequências do ficheiro Original. */
    FILE *rle; /**< Ficheiro original comprimido em compressão RLE. */
    FILE *rleFreqs; /**< Ficheiro com as frequências do ficheiro RLE. */
} *Fic;

/**
 * @brief Struct com informações sobre o ficheiro original.
 *
 */
typedef struct ficheiroInf {
    char *nomeFicheiro;
    Fic ficheiros; /**< Struct com os ficheiros gerados. */
    unsigned long long int tamanhoTotal; /**< Tamanho total do ficheiro(em Bytes). */
    unsigned long int tamanhoBloco; /**< Tamanho de todos os blocos antes do último. */
    long int tamanhoUltimoBloco; /**< Tamanho do último bloco. */
    unsigned long long int numBloco; /**< Número de blocos do ficheiro. */
} *FicheiroInf;

/**
 * @brief Struct com informações sobre cada bloco(informações para os ficheiros)
 *
 * Decidimos só gerar um ficheiro de frequências.
 * Por isso, o buffer das frequências ou é do ficheiro original
 * ou do ficheiro RLE.
 *
 */
typedef struct infosBloco {
    Byte *BufferSimbolos; /**< Buffer com os símbolos da compressão RLE(NULL se a mesma não for efetuada. */
    Byte *BufferFreqs; /**< Buffer com as frequências(ou é do ficheiro original, ou é do ficheiro RLE. */
    unsigned long long int tamanhoBufferRle; /**< Tamanho do buffer da compressão RLE. */
    unsigned long long int tamanhoBufferFreqs; /**< Tamanho do buffer das frequências. */
} *InfosBloco;

/**
 * @brief Função que cria uma string com a extensão concatenada ao nome do ficheiro.
 * @param String com o nome do ficheiro.
 * @param String com o tipo da extensão.
 * 
 */
char *nomeFicheiroExtensao(const char *nomeFicheiro,const char *extensao);

/**
 * @brief Função que gera a struct com a informação do ficheiro original.
 * @param nomeFicheiro String com o nome do ficheiro.
 * @param tamanhoBloco Tamanho que cada bloco(excluindo o último) vai ter.
 * @return Struct com as informações do ficheiro original.
 *
 */
FicheiroInf NBlocos(char *nomeFicheiro, unsigned long int tamanhoBloco);

/**
 * @brief Função que calculo o tamanho de um bloco.
 * @param Struct com as Informações sobre o ficheiro original.
 * @param numBloco O número do bloco do qual se está a ver o tamanho.
 * @return O tamanho do bloco analisado.
 * 
 */
unsigned long int tamanhoBloco(FicheiroInf fInf, unsigned long long numBloco);

/**
 * @brief Função que passa a informação de um bloco do ficheiro para um buffer de unsigned chars.
 * @param f Apontador para o ficheiro.
 * @param tamanhoBloco O tamanho solicitado para os blocos
 * @return Array com a informação.
 */
Byte *leituraFicheiro(FILE *f, unsigned long int tamanhoBloco);

/**
 * @brief Função que escreve num ficheiro o buffer.
 * @param f Apontador para o ficheiro.
 * @param buffer O buffer.
 * @param tamanho Tamanho do buffer
 * 
 */
void escritaFicheiro(FILE *f, Byte *buffer, unsigned long int tamanho);

/**
 * @brief Função que apaga a struct com as informações dos buffers de um bloco.
 * @param infosBloco Struct com o buffers e tamanho dos mesmos, a ser apagada.
 * 
 */
void libertarEspacoInfosBloco (InfosBloco infosBloco);


/**
 * @brief Função que apaga a struct com a informação do ficheiro original.
 * @param fInf Struct a ser apagada.
 *
 */
void freeFicheiroInf(FicheiroInf fInf);


/**
 * @brief Função que escreverá no terminal os ficheiros que foram gerados.
 * @param fInf Struct com informações sobre o ficheiro original.
 * @param RleEfetuado Para saber se o RleEfetuado.
 *
 */
void ficheiros_gerados(FicheiroInf fInf, unsigned char RleEfetuado);

/**
 * @brief Função para escrever no terminal a informação geral sobre o Módulo.
 * @param fInf Struct com a informação do ficheiro original.
 * @param tamanhoRle Tamanho do ficheiro RLE.
 * @param inicio Para saber o inicio do Clock.
 * @param fim Para saber o fim do Clock.
 *
 */
void infoTerminal(FicheiroInf fInf,unsigned long long tamanhoRle, clock_t inicio, clock_t fim);

#endif //MODULO_F_FUNCOESAUXILIARES_H
