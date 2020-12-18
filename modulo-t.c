#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shafa.h"
#include "modulo-t.h"

LISTA crialista () {
    return NULL;
}

LISTA inserecabeca ( LISTA L , int s , int f , char * c ) {

    LISTA novo = malloc(sizeof(struct lista));
    novo->simbolo = s;
    novo->frequ = f;
    novo->codSF = c;
    novo->prox = L;

    return novo;
}

/* Funde duas listas ordenadas uma com a outra, formando outra lista ordenada
   Faz isso avançando em cada uma das listas e colocando elementos na lista maior  */
LISTA SortedMerge( LISTA a, LISTA b)
{ 
    LISTA result = NULL; 
  
    // Quando uma das listas é vazia
    if (a == NULL) 
        return (b); 
    else if (b == NULL) 
        return (a); 
  
    // Vai fundindo as listas, usando recorrência
    // Escolhe o valor maior para colocar e avança nessa lista
    if ( a->frequ > b->frequ ) { 
        result = a; 
        result->prox = SortedMerge(a->prox, b); 
    } 
    else { 
        result = b; 
        result->prox = SortedMerge(a, b->prox); 
    } 
    return (result); 
}

/* Separa a lista em duas metades e retorna as duas listas usando parâmetros de referência
   Se o comprimento da lista for ímpar, o nodo extra vai para a primeira lista
   Usa a estratégia de avançar um apontador duas vezes mais rápido que o outro,
   de modo a um chegar ao meio quando o outro chega ao fim.  */
void Divisao( LISTA source , LISTA * a , LISTA * b ) 
{ 
    LISTA l2; 
    LISTA l1; 
    l1 = source; 
    l2 = source->prox; 
  
    // Avança 'l2' dois nodos, e avança 'l1' um nodo
    while ( l2 != NULL ) { 
        l2 = l2->prox; 
        if ( l2 != NULL ) { 
            l1 = l1->prox; 
            l2 = l2->prox; 
        }
    } 
  
    // 'l1' é antes do meio da lista, portanto divide-se em dois aí
    *a = source;
    *b = l1->prox;
    l1->prox = NULL;
} 

void MergeSort( LISTA * L )
{
    LISTA * a; 
    LISTA * b; 

    // Caso a lista tenha 0 ou 1 elementos, fica igual
    if (( *L == NULL ) || ( (*L)->prox == NULL )) { 
        return;
    } 
  
    // Dividimos a lista em duas sublistas
    Divisao( *L , a , b ); 
  
    // Fazemos a MergeSort de cada uma das sublistas
    MergeSort(a); 
    MergeSort(b); 
  
    // No fim, fundimos as duas listas ordenadas uma com a outra
    *L = SortedMerge( *a , *b ); 
}

LISTA metenalista ( int * arr , LISTA L ) {
    int i;

    for ( i = 255 ; i >= 0 ; i-- )
        L = inserecabeca ( L , i , arr[i] , NULL ); //o 2 significa que ainda não tem um código SF definido

    return L;
}

LISTA * freqread ( char * aa ) {
    char * a ;
    int val = 1 ;
    char * p ;
    int arrobacheck = 0 ;
    int i , j ;
    int * arr ;
    strcpy( a , aa );
    for ( i = 0 , j = 0 ; val != 0 ; i++ ) {
        for ( ; a[i] == ';' || a[i] == '@' ; i++ , j++ ) {
            if ( a[i] == '@' && arrobacheck == 1 ) { val = 0 ; break ; }
            if ( a[i] == '@' && arrobacheck == 0 ) arrobacheck = 1 ;
            p = &a[i+1] ;
            val = atoi(p);
            arr[j] = val;
        } 
    }

    LISTA ll = crialista() ;
    ll = metenalista ( arr , ll ) ;

    return (&ll) ;
}

void moduleTMain ( char * frq ) {

/*
@<R|N>@[número_de_blocos]@[tamanho_bloco_1]@[frequência_símbolo_0_bloco_1]
;[frequência_símbolo_1_bloco_1];[…];[frequência_símbolo_255_bloco_1]@[tam_
bloco_2]@[frequência_símbolo_0_bloco_2];[frequência_símbolo_1_bloco_2];[…]
;[frequência_símbolo_255_bloco_2]@[…]@0
*/

    int na = 0;                                              // numero de @
    int j;                                                   // índice do array frq no primeiro for
    int i;                                                   // índice do array frq no segundo for
    LISTA * freq;                                            // array onde vamos colocar as frequências

   // este for serve apenas para sabermos qual o endereço onde podemos ir para o segundo for
   // este for está a "saltar" as informações dos 2 primeiros separadores de (@),
   // faz com que o for seguinte veja apenas a informação de cada um dos blocos
    for ( j = 0 ; na < 3 ; j++ )
        if (frq[j] == '@') 
            na++;

    // este for serve para vermos um bloco de cada vez. Ele acaba quando temos "@0"
    for ( i = j ; frq[i+1] != '0' ; i++ ) {
        i++;                                                 // avança para a informação do tamanho do bloco, que vamos ignorar
        freq = freqread ( frq );                             // pegar nesta parte do array de char e transforma-la numa lista ligada de inteiros
        for ( ; frq[i] != '@' ; i++ );                       // vemos quando acaba a informação do tamanho do bloco, que estamos a ignorar
        
    // fazer uma ordenação eficiente da lista através das frequenâncias
    MergeSort ( freq );

    // no fim de tudo, é necessário desfazer a lista ligada
    }
}