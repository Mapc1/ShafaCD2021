#ifndef __SHAFA_H__
#define __SHAFA_H__

/**
 * @brief Macros definidas
 * 
 * @param BUFFSIZE  Tamanho dos buffers
 * @param NSIMBOLOS Número de símbolos
 * @param NTHREADS  Número de threads
 * @param BILLION   1 milhar de Milhão
*/
#define BUFFSIZE 1024
#define NSIMBOLOS 256
#define NTHREADS 8
#define BILLION 1000000000.0

/**
 * @brief Ajuda para ser escrita caso não se forneçam argumentos
*/
#define HELP "Shafa - Programa de compressão/descompressão\n\n"\
             "Comandos: shafa -m [t|f|c|d] [-b K|m|M] [-c r]\n"\
             "          -m [t|f|c|d]    Executa o módulo [t|f|c|d]\n"\
             "          -b [K|m|M]      Especifica o tamanho dos blocos para análise\n"\
             "          -d r            Descomprime o ficheiro a partir de um ficheiro rle e outro freq\n"\
             "          -c r            Força a compressão RLE\n"

/**
 * @brief Opções que são passadas ao programa
 * 
 * @param fileIN   Nome do ficheiro fornecido como argumento
 * @param fileOUT  Nome do ficheiro final
 * @param fileCOD  Nome do ficheiro cod
 * @param fileRLE  Nome do ficheiro rle
 * @param fileFREQ Nome do ficheiro freq
 * @param modX     Módulo a ser executado
 * @param optX     Opções passadas ao programa
*/
typedef struct {
  char *fileIN,              
       fileOUT[BUFFSIZE],     
       fileCOD[BUFFSIZE],    
       fileRLE[BUFFSIZE],
       fileFREQ[BUFFSIZE];   
  int modT, modD, modF, modC; 
  char optB, optC, optD;      
} Options;

/**
 * @brief Flag para errorOpenFile que indica se o erro
 *        foi a abrir ou a criar o ficheiro
*/ 
typedef enum {
  READ,
  WRITE
} Flag;

/**
 * @brief Lista ligada que guarda o nome dos ficheiros criados
 * 
 * @param fileName Nome do ficheiro
 * @param next     Proximo FileCreated
*/ 
typedef struct fileCreated{
  char fileName[BUFFSIZE];
  struct fileCreated *next;  
}FileCreated;

/**
 * @brief Imprime a data atual
*/ 
void data();

/**
 * @brief Recebe os argumentos do programa e 
 *        passa-os para uma struct Options para 
 *        serem interpretados pelo resto do programa
 * 
 * @param argc Número de argumentos
 * @param argv Argumentos do programa
 * 
 * @return Endereço da struct criada
*/ 
Options *getOpts(int argc, char *argv[]);

/**
 * @brief Inicializa os dados de opts
 * 
 * @return 1 em caso de sucesso caso contrário retorna 0
*/ 
int initOpts(Options *opts);

/**
 * @brief Remove o sufixo de src e guarda em dest
 * 
 * @param src  Array original
 * @param dest Array para guardar o resultado
 *
 * @return Endereço de dest
*/ 
char *removeSufix(char *dest, char *src);

/**
 * @brief Substitui o sufixo de fileName por sufx
 *        e guarda o resultado em dest. De seguida abre-o em modo mode
 * 
 * @param dest     Array a guardar o resultado
 * @param fileName Array do ficheiro original
 * @param mode     Modo a abrir o ficheiro
 * @param sufx     Sufixo para substituir
 * 
 * @return File pointer para o ficheiro aberto 
*/ 
FILE *getFile(char *dest, char *fileName, char *mode, char *sufx);

/**
 * @brief Reporta o erro a abrir o ficheiro fileName
 *        e fecha todos os ficheiros abertos
 * 
 * @param fileName Nome do ficheiro a reportar o erro
 * @param flag     Flag de abertura/escrita
 * @param fpSF     Ficheiro shafa
 * @param fpRLE    Ficheiro rle
 * @param fpCOD    Ficheiro cod
 * @param fpFREQ   Ficheiro freq
 * @param fpOut    Ficheiro final
*/ 
void errorOpenFile (char *fileName, Flag flag, FILE *fpSF, FILE *fpRLE, FILE *fpCOD, FILE *fpFREQ, FILE *fpOut);

/**
 * @brief Adiciona o newFile a lista fileCreated
 * 
 * @param fileCreated Lista ligada com os nomes dos ficheiros criados
 * @param newFile     Nome do ficheiro a adicionar
*/ 
void addFilesCreated(FileCreated **fileCreated, char *newFile);
#endif //__SHAFA_H__
