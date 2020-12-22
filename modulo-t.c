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

int somal ( LISTA * l , LISTA * l2 ) {
    int s = 0 ;

    for ( ; l != l2 ; *l=(*l)->prox )
        s += (*l)->frequ ;

    s += (*l2)->frequ ;

    return s ;
}

LISTA * melhordivisao ( LISTA * l , LISTA * l2 ) {
    LISTA * div = l ;
    LISTA * divan = l;
    int g1 = 0 ;
    int total = somal ( l , l2 );
    int mindif = total ;
    int dif = total ;

    do {
        g1 += (*div)->frequ ;
        dif = abs ( 2 * g1 - total ) ;
        if ( dif < mindif ) {
            divan = div ;
            *div = (*div)->prox ;
            mindif = dif ;
        }
        else dif = mindif + 1 ;
    } while ( dif != mindif ) ;

    return divan ;
}

char * addSF ( char * c , char d ) {
    int i ;
    
    for ( i = 0 ; c[i] ; i++ ) ;

    c[i] = d ;

    return c ;
}

void ShannonFannon ( LISTA * l ,  LISTA * l2 ) {
    LISTA * lfix = l ;
    LISTA * ldiv = l ;

    if ( l != NULL ) {
        LISTA * div = melhordivisao ( ldiv , l2 ) ;
        for ( ; &l != &div ; *l = (*l)->prox )
            (*l)->codSF = addSF ( (*l)->codSF , 0 );
        for ( ; l != l2 ; *l = (*l)->prox )
            (*l)->codSF = addSF ( (*l)->codSF , 0 );
        
        ShannonFannon ( lfix , div ) ;
        ShannonFannon ( div , NULL ) ;
    }
}

/* Funde duas listas ordenadas uma com a outra, formando outra lista ordenada
   Faz isso avançando em cada uma das listas e colocando elementos na lista maior  */
LISTA SortedMerge ( LISTA a , LISTA b , int fl ) { 
    LISTA result = NULL; 
  
    // Quando uma das listas é vazia
    if (a == NULL) 
        return (b); 
    else if (b == NULL) 
        return (a); 
  
    // Vai fundindo as listas, usando recorrência
    // Escolhe o valor maior para colocar e avança nessa lista
    if ( ( fl == 1 && a->frequ > b->frequ ) || ( fl == 2 && a->simbolo < b->simbolo ) ) { 
        result = a; 
        result->prox = SortedMerge ( a->prox , b , fl ) ; 
    }
    else { 
        result = b; 
        result->prox = SortedMerge ( a , b->prox , fl ) ; 
    }

    return result ; 
}

/* Separa a lista em duas metades e retorna as duas listas usando parâmetros de referência
   Se o comprimento da lista for ímpar, o nodo extra vai para a primeira lista
   Usa a estratégia de avançar um apontador duas vezes mais rápido que o outro,
   de modo a um chegar ao meio quando o outro chega ao fim.  */
void Divisao ( LISTA source , LISTA * a , LISTA * b , int fl ) { 
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

LISTA * MergeSort ( LISTA * L , int fl ) {
    LISTA * a; 
    LISTA * b; 

    // Caso a lista tenha 0 ou 1 elementos, fica igual
    if (( *L == NULL ) || ( (*L)->prox == NULL )) { 
        return;
    } 
  
    // Dividimos a lista em duas sublistas
    Divisao ( *L , a , b , fl ) ; 
  
    // Fazemos a MergeSort de cada uma das sublistas
    MergeSort ( a , fl ) ; 
    MergeSort ( b , fl ) ; 
  
    // No fim, fundimos as duas listas ordenadas uma com a outra
    *L = SortedMerge ( *a , *b , fl ) ;

    return L ;
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

void moduleTMain ( FILE ff ) {

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
    char * final ;

    // para começar, precisamos de uma função que transforme o FILE num array de chars, exatamente igual ao FILE.
    char * frq;
    frq = // . . .

    // este é o que nos indica se é uma codificação rle ou nao, 
    // só precisamos deste char no fim, para colocar no array que dará origem ao ficheiro cod
    char rn = frq[1] ;

    // indica-nos o número de blocos
    int nb ;
    nb = atoi ( &frq[3] ) ;

    // este for serve para vermos um bloco de cada vez. Ele acaba quando temos "@0"
    for ( i = 3 ; frq[i+1] != '0' ; i++ ) {
        i++;                                                 // avança para a informação do tamanho do bloco, que vamos ignorar
        int tb = atoi ( &frq[i] ) ;                          // o tamanho do bloco
        for ( ; frq[i] != '@' ; i++ );                       // vemos quando acaba a informação do tamanho do bloco, que estamos a ignorar
        freq = freqread ( frq );                             // pegar nesta parte do array de char e transforma-la numa lista ligada de inteiros
        
        // fazer uma ordenação eficiente da lista através das frequências
        freq = MergeSort ( freq , 1 ) ;

        //atribuir códigos Shannon-Fannon aos símbolos
        ShannonFannon ( freq , freq ) ;

        //ordenar a lista em função dos simbolos
        freq = MergeSort ( freq , 2 ) ;

        //

// @<R|N>@[número_de_blocos]@[tamanho_bloco_1]@<0|1>*;[…];<0|1>*@[tamanho_bloco_2]@<0|1>*;[…];<0|1>*@[…]@0 

    // no fim de tudo, é necessário desfazer a lista ligada
    }

    //função que transforma o array de chars que temos num ficheiro
}