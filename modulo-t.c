#include <stdio.h>
#include <string.h>
#include "shafa.h"

void moduleTMain( /* char * frq */ ) {

/*
@<R|N>@[número_de_blocos]@[tamanho_bloco_1]@[frequência_símbolo_0_bloco_1]
;[frequência_símbolo_1_bloco_1];[…];[frequência_símbolo_255_bloco_1]@[tam_
bloco_2]@[frequência_símbolo_0_bloco_2];[frequência_símbolo_1_bloco_2];[…]
;[frequência_símbolo_255_bloco_2]@[…]@0
*/

    int na = 0; //numero de @
    int j; // índice do array frq no primeiro for
    int i; // índice do array frq no segundo for

// este for serve apenas para sabermos qual o endereço onde podemos ir para o segundo for
// este for está a "saltar" as informações dos 2 primeiros separadores de campos (@),
// faz com que o for seguinte veja apenas a informação de cada um dos blocos
    for ( j = 0 ; na < 3 ; j++ )
        if (frq[j] == '@') 
            na++;

// este for serve para vermos um bloco de cada vez. Ele acaba quando temos "@0"
    for ( i = j ; frq[i+1] != '0' ; i++ ) {
        i++; // avança para a informação do tamanho do bloco, que vamos ignorar
        for ( ; frq[i] != '@' ; i++ ); // vemos quando acaba a informação do tamanho do bloco, que estamos a ignorar
    // ordenar os simbolos por ordem decrescente de frequências
    // pegar nesta parte do array de char e transforma-la num array de inteiros
    // fazer uma ordenação eficiente do array de inteiros ()
    }
}