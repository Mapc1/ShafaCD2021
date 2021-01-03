/**
 * @file moduloF.h
 * @author Miguel Martins, a93280
 * @author Gonçalo Rodrigues, a93286
 * @date 2 janeiro 2021
 * @brief Ficheiro que contém as funções principais do nosso módulo.
 *
 * A funções deste ficheiro usam, principalmente, funções do
 * processamento para gerar os ficheiros necessários do
 * nosso módulo, bem como para imprimir no terminal a
 * informação pertinente.
 *
 */

#ifndef __MODULO_F__
#define __MODULO_F__

#include "../shafa.h"
#include "funcoesAuxiliares.h"

/**
 * @brief 64 kB
 *
 */
#define K 655360
/**
 * @brief 8 MB
 *
 */
#define m 8388608
/**
 * @brief 64 MB
 *
 */
#define M 67108864
/**
 * @brief Compressão forçada. 
 *
 */
#define r 1

/**
 * @brief Função "main" do módulo f.
 * @param opts Apontador para a struct com as informações necessárias para executar o módulo.
 * 
 */
void moduleFMain(Options *opts);

/**
 * @brief Função para calcular as frequências.
 * @param orig Apontador para o ficheiro original.
 * @param fInf Struct com informações sobre o ficheiro original.
 * @param compressaoForcada Para saber se a compressão foi ou não forçada.
 * @return O tamanho do ficheiro RLE.
 *
 */
unsigned long long int *calculoFrequencias(FILE *orig, FicheiroInf fInf, char compressaoForcada);

/**
 * @brief Função que realmente processa a informação da nossa "main".
 * @param nomeFicheiro Nome do ficheiro a ser processado.
 * @param compressaoForcada Para saber se a compressão foi ou não forçada.
 * @param tamanhoBloco O tamanho que terão os blocos, aquando da divisão do ficheiro.
 * @return 0 se foi executada até ao final.
 *
 */
int moduloF(char *nomeFicheiro, char compressaoForcada, unsigned long tamanhoBloco);

/**
 * @brief Função para calcular as frequências de cada bloco
 * @param orig Apontador para o ficheiro original.
 * @param fInf Struct com informações sobre o ficheiro original.
 * @param numBloco Para saber qual o bloco que estamos a analisar(Na verdade estamos a analisar o bloco numBloco+1). 
 * @param compressaoForcada para saber se a compressão foi ou não forçada.
 * @param tamanhoRleAcumulado Para saber o tamanho que o RLE tem àquele momento(até àquele bloco).
 *
 */
void calculoFrequenciasBloco(FILE *orig, FicheiroInf fInf, unsigned long long numBloco, char compressaoForcada, unsigned long long **tamanhoRleAcumulado);

#endif //MODULO_F_MODULOF_H
