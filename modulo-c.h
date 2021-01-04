/*
Diogo Pires, a93239
Jorge Melo, a93308
Data de criação: 16 de dezembro de 2020
Ficheiro com as funções principais do nosso módulo.
Asa funções aqui presentes estão descritas no relatório
*/
#ifndef __MODULE_C__
#define __MODULE_C__

#include "shafa.h"
#include <stdlib.h>
#include <stdio.h>
#define VALORES_ASCII 257

//Estrutura de dados utilizada para fazer multi-threading. 
struct thread_data{
    unsigned char *buffer_cod; //Cópia dum bloco do ficheiro .cod
  //  int valoresASCII; //Número de valores ASCII, é constante
    char * fim_cod; //Endereço de controlo da estrutura de, da função ler_bloco_ficheiro_cod
    unsigned char *buffer; //Cópia dum bloco do ficheiro original
    int tam_Ori; //Tamanho do bloco lido do ficheiro original. 
    int tam_Shaf; //Tamanho do bloco do ficheiro .shaf codificado pelos outros elementos desta struct
    unsigned char *arr_final;  //Apontador para o array final que codifica o bloco descrito nesta estrutura
    int bloco_atual;   //Bloco atual que esta estrutura de dados descreve
} ;


//Função que converte um array de 8 char's e converte para 1 byte (char)
//char *byte -> Array de 8 char's que só contem "1" ou "0". 
//char devolvido -> Byte que codifica o byte recebido
char converte_Para_Byte (char *byte);

//Função que liberta espaço da memória como arrays de arrays, explicadas no relatório com o nome "cod"
//char **array -> Array que vai ter free em cada uma das posições
//int valoresLidos -> Número de posições que esse array tem.
void Limpa_Array_Arrays(char **array, int valoresLidos);

//Função que lê um bloco do ficheiro .cod e converte para um array de arrays, explicadas no relatório com o nome "cod"
//char * buffer -> buffer que contêm o ficheiro .cod. 
//char * fim -> Endereço de controlo de fim do array
//char ** -> Estrutura de dados "cod"
char ** ler_bloco_ficheiro_cod (char * buffer, char * fim);


//Função que divide um buffer, com o ficheiro .cod, e divide em blocos.
//char * buffer -> String que contêm o ficheiro .cod integral.
//int * pos_buffer -> Posição atual no buffer que será analisada. Isto é, começo do bloco.
//int * tamanhos -> Array de números inteiros onde se guarda o tamanho do bloco que será analisado nas outras funções.
// Esse tamanho está registado no ficheiro .cod e é relativo ao ficheiro original.
//int * bloc_atual -> Guarda qual o bloco atual.
//char * result -> Guarda uma cópia do bloco atual, isolada do resto do ficheiro. 
//Retorna quantas posições avançou no buffer. 
int ordena_cod(char * buffer, int * pos_buffer, int * tamanhos, int * bloc_atual, char * result);

//Função parecida com a "strcpy", mas com apontadores.
//char * copias -> Onde é guardado o array original.
//char * temp -> Array original.
void  strcpyApontadores(char * copias, char * temp);

//Função que lê o ficheiro .cod e separa em diferentes blocos. Esses blocos vão para a função ler_bloco_ficheiro_cod.
// int * tamanhos -> Endereço para guardar os tamanhos de cada bloco do ficheiro original.
// FILE * fp -> Apontador para o ficheiro .cod, usado para ler o ficheiro.
// int num_bloc -> Número de blocos do ficheiro original.
// int tam_ficheiro -> Tamanho do ficheiro .cod.
char ** le_PontoCod ( int * tamanhos, FILE * fp, int num_bloc, unsigned long long int tam_ficheiro);

//Função que tem multithreading. Ela converte o ficheiro original numa string de saída. 
//Essa string é escrita no ficheiro de saída.
//A struct guarda todos os argumentos. Esses argumentos estão caraterizados na descrição da struct.
void *PontoShafa (void * testeT);
void infoTerminalINI(int num_blocs);

int moduleCMain (Options * opts, FileCreated **list);


#endif
