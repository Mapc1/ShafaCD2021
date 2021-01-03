#ifndef MODULO_T_MODULO_T_H
#define MODULO_T_MODULO_T_H

/*
typedef struct Parcelas {
    long long int simbolo;
    long long int frequ;
    long long int codSF;
} Parcela ;

typedef struct lista {
    struct Parcelas parc;
    struct lista * prox;
} *LISTA;
*/

typedef struct lista {
    long long int simbolo;
    long long int frequ;
    char * codSF;
    struct lista * prox;
} *LISTA;

LISTA crialista () ;

LISTA inserecabeca ( LISTA L , long long int s , long long int f ) ;

char * detectfreq ( char * freq ) ;

void escreveFile ( char * freq ) ;

long long int somal ( LISTA * l , long long int ai , long long int af ) ;

long long int melhordivisao ( LISTA * l , long long int ai , long long int af ) ;

void addSF ( char * c , char d ) ;

void ShannonFannon ( LISTA * l , long long int ai , long long int af ) ;

LISTA SortedMerge ( LISTA a , LISTA b , long long int fl ) ;

void Divisao ( LISTA source , LISTA * a , LISTA * b ) ;

void MergeSort ( LISTA * L , long long int fl ) ;

LISTA metenalista ( long long int * arr , LISTA L ) ;

//long long int finalefree ( LISTA * L , char * final , long long int ii , long long int sizefi ) ;

long long int * freqread ( char * aa ) ;

long long int contan ( LISTA * l ) ;

void moduleTMain ( char * ff ) ;

#endif
