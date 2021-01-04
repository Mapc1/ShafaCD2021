#ifndef __MODULE_D__
#define __MODULE_D__
#include "shafa.h"
#include <stdlib.h>
#include <stdio.h>
/**
 * @file modulo-d.h
 * @authors Marco Costa, A93283
 * @authors Daniel Azevedo, A93324
 * @date 03/01/2021
 * 
 * @brief Este módulo é responsável pela descodificação RLE e Shannon-Fano
 * 
 * @note  Faz proveito de multithreading e é compatível tanto com Windows e Linux
*/

// É pá somos nós...
#define AUTHORS "Autores: Marco Costa: A93283  Daniel Azevedo: A93324 \n\n"

/**
 * @brief Tipo de pré compressão utilizada antes da descodificação Shannon-Fano
 *        O programa usa esta informação para decidir se será necessária descompressão RLE ou não
*/
typedef enum {
  RLE = 'R',
  NONE = 'N'
} Precomp;


/** 
 * @brief Árvore binária simples
 * 
 * @details Começa-se na raíz e lê-se um bit caso este seja 1 descemos a árvore pela direita,
 *          caso contrário vai-se pela esquerda. De seguida verifica-se se o nodo é uma folha
 *          se sim então lê-se o caracter guardado nesse nodo e guarda-se num buffer para depois ser escrito,
 *          senão repete-se o processo
 * 
 * @param c     Caracter cujo código Shannon-Fano segue um caminho da raíz a esta folha
 * @note        Apenas as folhas contém informação!  
 * @param left  Pointer para a sub-árvore esquerda
 * @param right Pointer para a sub-árvore direita
*/
typedef struct abin {
  unsigned char c;
  struct abin *left;
  struct abin *right;
} ABin;


/**
 * @brief Struct que contém informação relativa a um bloco
 * 
 * @param blockNum  Número do bloco
 * @param oldSize   Tamanho original do bloco
 * @param newSize   Tamanho do bloco após a descodificação Shannon-Fano
 * @param codes     Pointer para a árvore binária com os códigos Shannon-Fano
 * @param next      Pointer para o próximo BlockData 
*/
typedef struct blockData{
  int blockNum, oldSize, newSize;
  ABin *codes;
  struct blockData *next;
} BlockData;

/**
 * @brief Struct com a pré compressão utilizada e um pointer para o primeiro BlockData
 * 
 * @param compress Tipo de precompressão
 * @param first    Pointer para o primeiro BlockData  
*/
typedef struct {
  Precomp compress;
  BlockData *first;
} FileData;

/**
 * @brief Struct com informação do bloco para os threads poderem executar a descodificação do bloco
 * 
 * @param buffer    Buffer com o bloco do ficheiro shafa por descodificar
 * @param decoded   Buffer que os threads usam para guardar o bloco descodificado
 * @param blockSize Tamanho do bloco descodificado. Este valor é usado na escrita do decoded
 *                  para se saber quantos blocos se deve escrever
 * @param ready     Flag que indica se um bloco está pronto a ser escrito ou não
 * @param threadID  Id do thread que fez a descodificação do bloco. 
 *                  É usado na escrita para libertar o pointer do array ocupation correto
 * @param next      Pointer para o próximo BlockBuff
*/
typedef struct blockBuff {
  char *buffer, *decoded;
  int blockSize, ready, threadID;
  struct blockBuff *next;
} BlockBuff;


/**
 * @brief Struct com a cabeça da lista (Era para remover mas o 
 *        código deixa de funcionar e não tenho tempo)
*/
typedef struct buffQueue {
  BlockBuff *head;
} BuffQueue;

/**
 * @brief Struct com todos os argumentos passados a cada thread
 * 
 * @param blocBuff  Pointer para o BlockBuff do bloco que o thread tem que descodificar
 * @param block     Blockdata com informação relativa ao bloco a descodificar
 * @param ocupation Pointer para o array de Args que indicam a ocupação de cada thread
*/
typedef struct args{
  BlockBuff *blockBuff;
  BlockData *block;
  struct args **ocupation;
} Args;

/**
 * @brief Função principal do módulo, responsável por chamar as funções apropriadas
 * 
 * @param opts Pointer para a struct Options
 * @param list Pointer para o pointer da struct list que é usada
 *             para guardar os nomes dos ficheiros criados 
*/
void moduleDMain(Options *opts, FileCreated **list);

/**
 * @brief Função responsável pela descodificação RLE
 * 
 * @details Esta função recebe um file pointer para um ficheiro RLE 
 *          e descodifca-o escrevendo-o no file pointer fout. 
 *          O fpFREQ só é necessário caso seja usado o comando "-d r" para se saber o tamanho dos blocos
 *          Em contrapartida se o comando não for usado fileData é usado para atingir o mesmo objetivo
 * 
 * @param fpRLE    Ficheiro RLE a ser descodificado
 * @param fpOut    Ficheiro a ser escrito
 * @param fpFREQ   Ficheiro freq com informação relativa ao tamanho dos blocos
 * @param fileData Informação relativa ao ficheiro é usado para se saber o tamanho de cada bloco
 *                 antes/depois da descompressão
*/
void decodeRLE(FILE *fpRLE, FILE *fpOut, FILE *fpFREQ, FileData *fileData);

/**
 * @brief Função responsável pela descodificação Shannon-Fano
 * 
 * @details Esta função tira proveito do multithreading para acelerar a descodificação
 * 
 * @param fpSF     Ficheiro Shafa a ser descodificado
 * @param fpOut    Ficheiro a ser escrito
 * @param fileData Informação relativa ao ficheiro é usado para se saber o tamanho de cada bloco
 *                 antes/depois da descompressão
*/
FileData *decodeShafa(FILE *fpSF, FILE *fpOut, FileData *fileData);

/**
 * @brief Descodifica um bloco shafa
 * 
 * @details Esta função é passada para os threads através das funções
 *          pthread_create (em Linux) e CreateThread (em Windows)
 * 
 * @param arg Struct com os argumentos necessários para executar a função
 *            ver Args
*/
void *decodeSFBlock(void *arg);

/**
 * @brief Interpreta um ficheiro cod
 * 
 * @details Através de um ficheiro cod, esta função cria um FileData
 *          com informação relativa a cada bloco
 * 
 * @param fpCod Ficheiro cod a ser interpretado
 * 
 * @return FileData com informação relativa aos blocos do ficheiro shafa
*/
FileData *readCOD(FILE *fpCOD);

/**
 * @brief Cria uma árvore binária a partir do ficheiro cod e guarda em block
*/ 
void getCodes(FILE *fpCOD, BlockData *block);

/**
 * @brief Liberta a memoria guardada em block
 * 
 * @param block Bloco a libertar
*/
void freeBlockData (BlockData *block);

/**
 * @brief Cria um novo nodo ABin
 * 
 * @return Endereço do nodo criado
*/
ABin *createNode();



/**
 * @brief Lê os bytes até ao próximo '@'
 * 
 * @param fp Ficheiro a ler
 * 
 * @return Número de bytes lidos
*/
long readSection(FILE *fp, char *str);

/**
 * @brief Avança o fp até ao proximo '@'
 * 
 * @return Número de bytes lidos
*/
long skipsection(FILE *fp);

/**
 * @brief Lê os proximos dois bytes do ficheiro fpRLE e 
 *        guarda o simbolo em symbol e o número de repetições
 * 
 * @param fpRLE       Ficheiro RLE a ser lido
 * @param symbol      Pointer para guardar o simbolo
 * @param repetitions Pointer para guardar o número de repetições
 * 
 * @return Número de bytes lidos 
*/
long readRLECode(FILE *fpRLE, unsigned char *symbol, unsigned char *repetitions);

/**
 * @brief Lê o tamanho do bloco a partir do ficheiro freq
 * 
 * @param fpFREQ ficheiro freq a ser lido
 * 
 * @return Tamanho do bloco
*/
int readFreqBlockSize(FILE *fpFREQ);

/**
 * @brief Incializa os dados em data
*/
void initFileData(FileData *data);

/**
 * @brief Inicializa os dados em block
*/ 
void initBlock(BlockData *block);

/**
 * @brief Cria uma BuffQueue
 * 
 * @return Endereço da BuffQueue
*/
BuffQueue *initBuffQueue();

/**
 * @brief Recebe o array de Args e retorna o id do primeiro thread livre
 * 
 * @brief Array com Args passados aos threads
 * 
 * @return Id do primeiro thread livre
*/
int getFreeThread(Args **ocupation);

#endif //__MODULE_D__
