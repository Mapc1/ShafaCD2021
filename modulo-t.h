#ifndef MODULO_T_MODULO_T_H
#define MODULO_T_MODULO_T_H

#include "shafa.h"

typedef struct lista {
    int simbolo;
    long long int frequ;
    char * codSF;
    struct lista * prox;
} *LISTA;

LISTA crialista () ;

LISTA inserecabeca ( LISTA L , int s , long long int f ) ;

LISTA metenalista ( long long int * arr , LISTA L ) ;

char * detectfreq ( char * freq ) ;

void escreveFile ( char * freq , char * file , FileCreated ** list , int r ) ;

long long int somal ( LISTA * l , int ai , int af ) ;

int melhordivisao ( LISTA * l , int ai , int af ) ;

void addSF ( char * c , char d ) ;

void ShannonFannon ( LISTA * l , int ai , int af ) ;

LISTA SortedMerge ( LISTA a , LISTA b , int fl ) ;

void Divisao ( LISTA source , LISTA * a , LISTA * b ) ;

void MergeSort ( LISTA * L , int fl ) ;

long long int * freqread ( char * aa ) ;

long long int counti (long long int i , char * a ) ;

int countn ( LISTA * l ) ;

void moduleTMain ( Options * opts , FileCreated ** list ) ;

#endif