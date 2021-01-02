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

LISTA inserecabeca ( LISTA L , int s , int f ) ;

char * detectfreq ( char * freq ) ;

FILE * escreveFile ( char * freq ) ;

int somal ( LISTA * l , int ai , int af ) ;

int melhordivisao ( LISTA * l , int ai , int af ) ;

void addSF ( char * c , char d ) ;

void ShannonFannon ( LISTA * l , int ai , int af ) ;

LISTA SortedMerge ( LISTA a , LISTA b , int fl ) ;

void Divisao ( LISTA source , LISTA * a , LISTA * b ) ;

void MergeSort ( LISTA * L , int fl ) ;

LISTA metenalista ( int * arr , LISTA L ) ;

int finalefree ( LISTA * L , char * final , int ii ) ;

int * freqread ( char * aa ) ;

FILE * moduleTMain ( char * ff ) ;

#endif
