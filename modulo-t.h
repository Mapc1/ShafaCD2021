#ifndef MODULO_T_MODULO_T_H
#define MODULO_T_MODULO_T_H

/*
typedef struct Parcelas {
    int simbolo;
    int frequ;
    int codSF;
} Parcela ;

typedef struct lista {
    struct Parcelas parc;
    struct lista * prox;
} *LISTA;
*/

typedef struct lista {
    int simbolo;
    int frequ;
    char * codSF;
    struct lista * prox;
} *LISTA;

LISTA tornastruct ( char * arr );

LISTA insere_cabeca(LISTA L, void *valor);

LISTA SortedMerge( LISTA a, LISTA b);

void Divisao( LISTA source , LISTA * a , LISTA * b );

void MergeSort( LISTA * L );

LISTA metenalista ( int * arr , LISTA L );

LISTA * freqread ( char * aa );

void moduleTMain ( char * frq );

#endif