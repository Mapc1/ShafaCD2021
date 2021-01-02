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
 * @brief .
 * @param
 * @param
 * 
 */
char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao);

/**
 * @brief .
 * @param
 * @param
 *
 */
FicheiroInf NBlocos(char *nomeFicheiro, unsigned long int tamanhoBloco);

/**
 * @brief .
 * @param
 * @param
 * 
 */
unsigned long int tamanhoBloco(FicheiroInf fInf, unsigned long long numBloco);

/**
 * @brief .
 * @param
 * @param
 */
Byte *leituraFicheiro(FILE *f, unsigned long int tamanhoBloco);

/**
 * @brief .
 * @param
 * @param
 * @param
 * 
 */
void escritaFicheiro(FILE *f, Byte *buffer, unsigned long int tamanho);

/**
 * @brief .
 * @param
 * 
 */
void libertarEspacoInfosBloco (InfosBloco infosBloco);

<<<<<<< HEAD
/**
 * @brief .
 *
 */
=======
void freeFicheiroInf(FicheiroInf fInf);

>>>>>>> 65ee39796dc188ddff8e50abae07b45e54eb52b2
void data();

/**
 * @brief .
 * @param
 * @param
 *
 */
void ficheiros_gerados(FicheiroInf fInf, unsigned char RleEfetuado);

/**
 * @brief .
 * @param
 * @param
 * @param
 * @param
 *
 */
void infoTerminal(FicheiroInf fInf,unsigned long long tamanhoRle, clock_t inicio, clock_t fim);

#endif //MODULO_F_FUNCOESAUXILIARES_H
