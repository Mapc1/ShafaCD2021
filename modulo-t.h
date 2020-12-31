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

LISTA crialista () ;

LISTA insere_cabeca ( LISTA L , void * valor ) ;

char * detectfreq ( char * freq ) ;

FILE * escreveFile ( char * freq ) ;

int somal ( LISTA * l , LISTA * l2 ) ;

LISTA * melhordivisao ( LISTA * l , LISTA * l2 ) ;

char * addSF ( char * c , char d ) ;

void ShannonFannon ( LISTA * l ,  LISTA * l2 ) ;

LISTA SortedMerge ( LISTA a , LISTA b , int fl ) ;

void Divisao ( LISTA source , LISTA * a , LISTA * b ) ;

void MergeSort ( LISTA * L , int fl ) ;

LISTA metenalista ( int * arr , LISTA L ) ;

int finalefree ( LISTA * L , char * final , int ii ) ;

int * freqread ( char * aa ) ;

FILE * moduleTMain ( char * ff ) ;

#endif
