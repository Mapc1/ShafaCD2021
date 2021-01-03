#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "shafa.h"
#include "modulo-t.h"

LISTA crialista () {
    return NULL;
}

LISTA inserecabeca ( LISTA L , int s , long long int f ) {

    LISTA novo = malloc(sizeof(struct lista));
    novo->simbolo = s;
    novo->frequ = f;
    novo->codSF = calloc( 256 , sizeof(char));
    novo->prox = L;

    return novo;
}

LISTA metenalista ( long long int arr[] , LISTA L ) {
    int i;

    for ( i = 255 ; i >= 0 ; i-- )
        L = inserecabeca ( L , i , arr[i] ) ;

    return L ;

}

char * detectfreq ( char * freq ) { // Função que lê o conteúdo do ficheiro e passa-o para um array de char(string).

    char c ; // Variável que lê o conteúdo do ficheiro (valor de comparação para EOF)
    char *buffer = NULL; // Variável que guardará a string.
    size_t size = 0; // Variável que representa o tamanho para a string buffer.

    FILE *fp = fopen(freq, "r"); // Abrir o ficheiro para leitura.

    // As próximas linhas servem para a mandar o ficheiro para EOF.Isto serve para determinar o tamanho da nossa string
    // e o ftell serve para nos dizer quantos caracteres o nosso ficheiro possui para dar um tamanho à string.
    
    do {
        c = fgetc(fp) ;
    } while ( c != EOF ) ;
    
    size = ftell(fp); // Tamanho da string necessário.

    rewind(fp); // O ficheiro volta para o SEEK_SET,ou seja,o inicio do ficheiro,para o voltar a ler.

    buffer = malloc((size + 1) * sizeof(*buffer)); // Usando o size,determinamos o tamanho da nossa string,ou então,buffer.Usamos para o malloc para alocar espaço e sizeof para determinar o tamanho de cada elemento do buffer.

    fread ( buffer , size , 1 , fp ) ; // A partir do nosso file,iremos ler um 1 bloco de size bytes para cada incrementação de buffer.

    buffer[size] = '\0' ; // Colocar o último elemento ('\0') no array,para indicar o final.

    // Fechar o ficheiro.

    fclose(fp);

    //Devolver a string.

    return buffer;
}

void escreveFile ( char * freq ) { // Função que escreve uma string num ficheiro.

    FILE * fp;

    fp = fopen("ourbbb.txt.cod","w"); // Abrir o fichero para escrita(Neste caso também criamos o ficheiro se ele não existir.)

    fputs (freq,fp); // fputs é uma função que irá colocar o conteúdo de uma string no ficheiro,neste caso,a string freq para o ficheiro fp.

    fclose(fp); // Fechar o ficheiro fp.

    //return (fp); // Retornar fp.
}

long long int somal ( LISTA * l , int ai , int af ) {
    long long int s = 0 ;
    int i ;

    LISTA t = *l ;

    for ( i = 0 ; i < ai ; i++ , t = t->prox ) ;
    
    for ( ; i <= af ; i++ , t = t->prox )
        s += t->frequ ;

    return s ;
}

int melhordivisao ( LISTA * l , int ai , int af ) {
    int dv = ai ;
    int i ;
    long long int g1 = 0 ;
    long long int total = somal ( l , ai , af );
    long long int mindif = total ;
    long long int dif = total ;
    

    do {
        LISTA t = *l ;
        for ( i = 0 ; i < dv ; i++ , t = t->prox ) ;
        g1 = g1 + t->frequ ;
        dif = abs( 2 * g1 - total ) ;
        if ( dif <= mindif ) {
            dv++ ;
            mindif = dif ;
        }
        else dif = mindif + 1 ;
    } while ( dif == mindif ) ;

    return ( dv - 1 ) ;
}

void addSF ( char * c , char d ) {
    int i = 0 ;

    for ( ; c[i] ; i++ ) ;

    c = realloc ( c , (i+1)*sizeof(char) ) ;
    c[i] = d ;

}

void ShannonFannon ( LISTA * l , int ai , int af ) {
    int i ;

    if ( ai != af ) {
        int div = melhordivisao ( l , ai , af ) ;
        LISTA t = *l ;
        for ( i = 0 ; i < ai ; i++ , t = t->prox ) ;
        for ( ; i <= div ; i++ , t = t->prox )
            addSF ( t->codSF , '0' ) ;
        for ( ; i <= af ; i++ , t = t->prox ) 
            addSF ( t->codSF , '1' );
        
        ShannonFannon ( l , ai , div ) ;
        ShannonFannon ( l , div + 1 , af ) ;

    }

    return ;
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
void Divisao ( LISTA source , LISTA * a , LISTA * b ) { 
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

void MergeSort ( LISTA * L , int fl ) {
    LISTA a = crialista() ; 
    LISTA b = crialista() ; 

    // Caso a lista tenha 0 ou 1 elementos, fica igual
    if (( *L == NULL ) || ( (*L)->prox == NULL )) { 
        return ;
    } 
  
    // Dividimos a lista em duas sublistas
    Divisao ( *L , &a , &b ) ; 
  
    // Fazemos a MergeSort de cada uma das sublistas
    MergeSort ( &a , fl ) ; 
    MergeSort ( &b , fl ) ; 
  
    // No fim, fundimos as duas listas ordenadas uma com a outra
    *L = SortedMerge ( a , b , fl ) ;

}

long long int * freqread ( char * a ) {
    long long int val = 1 ;
    char * p ;
    int arrobacheck = 0 ;
    long long int i ;
    int j ;
    long long int * arr = malloc(256 * sizeof(long long int)) ;
    for ( i = 0 , j = 0 ; arrobacheck != 2 ; i++ ) {
        for ( ; a[i] == ';' || a[i] == '@' ; i++ , j++ ) {
            if ( a[i] == '@' && arrobacheck == 1 ) arrobacheck = 2 ;
            else {
                if ( a[i] == '@' && arrobacheck == 0 ) arrobacheck = 1 ;
                if ( a[i + 1] != ';') {   
                  p = &a[i+1] ;
                  val = atoi(p);
                  arr[j] = val;
                }
                else {
                  arr[j] = arr[j-1]; 
                }
            }
        }
    }
    return arr ;
}

long long int counti (long long int i , char * a ) {
    int arrobacheck = 0 ;
    long long int j ;

    for ( j = 0 ; arrobacheck != 1 ; j++ , i++ ) {
        if ( a[j] == '@') 
            arrobacheck = 1 ;
    }
    return i ;
}

int contan ( LISTA * l ) {

    LISTA t = *l ;
    int n = 0 ;

    for ( ; (t->frequ) != 0 && n < 226 ; n++ , t = t->prox ) ;

    return n;
}

void moduleTMain ( char * ff ) { //ff é o nome fo ficheiro .freq que usamos como argumento.

    clock_t tinicio = clock();
/*
@<R|N>@[número_de_blocos]@[tamanho_bloco_1]@[frequência_símbolo_0_bloco_1]
;[frequência_símbolo_1_bloco_1];[…];[frequência_símbolo_255_bloco_1]@[tam_
bloco_2]@[frequência_símbolo_0_bloco_2];[frequência_símbolo_1_bloco_2];[…]
;[frequência_símbolo_255_bloco_2]@[…]@0
*/

    long long int i , ii ;                                             // índice do array frq e do final, respetivamente
    char * final = malloc ( 3 * sizeof(long long int) ) ;              // array que vai dar origem ao ficheiro cod final
    long long int sizefi = 3 ;
    long long int B = 0 ;                                              // numero de blocos

    // para começar, precisamos de uma função que transforme o FILE num array de chars, exatamente igual ao FILE.
    char * frq;
    frq = detectfreq(ff);

    //coloca as informações iniciais no array final
    final[0] = '@' ;
    final[1] = frq[1] ;
    final[2] = '@' ;
    for ( ii = 3 ; frq[ii] != '@' ; ii++ ) {
        if ( ii >= sizefi ) {
            sizefi *= 2 ;
            final = realloc ( final , sizefi * sizeof(char) ) ;
        }
        final[ii] = frq[ii] ;
    }

    if ( ii > sizefi ) {
        sizefi *= 2 ;
        final = realloc ( final , sizefi * sizeof(char) ) ;
    }
    final[ii] = '@' ;

    i = ii ;
    //printf("1.   ii: %lld   i: %lld      sizefi: %lld\n%s\n\n", ii , i , sizefi , final) ;

    // este while serve para vermos um bloco de cada vez. Ele acaba quando temos "@0"
    while ( frq[i+1] != '0' ) {

        B++;

        // avança para a informação do tamanho do bloco
        i++; ii++;

        // põe a informação do tamanho do bloco no array final
        for ( ; frq[i] != '@' ; i++ , ii++ ) {
            if ( ii > sizefi ) {
                sizefi *= 2 ;
                final = realloc ( final , sizefi * sizeof(char) ) ;
            }
            final[ii] = frq[i] ;
        }
        //printf("2.   ii: %lld   i: %lld      sizefi: %lld\n%s\n\n", ii , i , sizefi , final ) ;
        final[ii] = '@' ;
        ii++;

        // pegar nesta parte do array de char e transforma-la numa lista ligada de inteiros
        long long int * arr ;
        arr = freqread ( &frq[i] ) ;
        i = counti ( i , &frq[i+1] ) ;

        //printf("3.   ii: %lld   i: %lld      sizefi: %lld\n%s\n\n", ii , i ,sizefi , final ) ;

        LISTA l = crialista() ;
        l = metenalista ( arr , l ) ;

        free (arr) ;
        
        // fazer uma ordenação eficiente da lista através das frequências
        MergeSort ( &l , 1 ) ;

        int n ;
        n = contan ( &l ) ;

        // atribuir códigos Shannon-Fannon aos símbolos
        ShannonFannon ( &l , 0 , (n-1) ) ;
        
        // ordenar a lista em função dos simbolos
        MergeSort ( &l , 2 ) ;
        
        // meter os códigos SF no array final e dá free da lista

        int k ;                   // para percorrer cada codSF, que sao arrays de chars

        // perceorre a lista, colocando os codSF
        while ( l->prox != NULL ) {
            char * c ;
            c = l->codSF ;

            // percorremos o codSF de cada nodo, colocando char a char no array final
            // de notar que se aquele nodo não tiver codSF, não é colocado nada
            for ( k = 0 ; c[k] ; k++ , ii++ ) {
                if ( ii >= sizefi ) {
                    sizefi *= 2 ;
                    final = realloc ( final , sizefi * sizeof(char) ) ;
                }
                final[ii] = c[k] ;
            }
            
            // liberta o nodo que já obtivemos a informação
            LISTA temporaria = l ;
            l = l->prox ;
            free ( temporaria->codSF ) ;
            free ( temporaria ) ;

            // coloca o ; no final, para separar os valores, ela só não é colocada no último
            if ( l != NULL ) {
                if ( ii >= sizefi ) {
                    sizefi *= 2 ;
                    final = realloc ( final , sizefi * sizeof(char) ) ;
                }
                final[ii] = ';' ;
                ii++ ;
            }
        }
        //ii = strlen ( final ) ;

        //printf("4.   ii: %lld   i: %lld      sizefi: %lld\n%s\n\n", ii , i , sizefi , final ) ;

        if ( ii >= sizefi ) {
            sizefi *= 2 ;
            final = realloc ( final , sizefi * sizeof(char) ) ;
        }
        //printf("4.5.   ii: %lld   i: %lld      sizefi: %lld    %c\n%s\n\n", ii , i , sizefi , final[ii+1] , final ) ;
        final[ii] = '@' ;
        //printf("5.   ii: %lld   i: %lld      sizefi: %lld    %c\n%s\n\n", ii , i , sizefi , final[ii+1] , final ) ;
        
// @<R|N>@[número_de_blocos]@[tamanho_bloco_1]@<0|1>*;[…];<0|1>*@[tamanho_bloco_2]@<0|1>*;[…];<0|1>*@[…]@0 

    }

    free (frq) ;

    ii++;
    final[ii] = '0';

    printf("6.   ii: %lld   i: %lld      sizefi: %lld    %c\n%s\n\n", ii , i , sizefi , final[ii] , final ) ;

    //função que transforma o array de chars que temos num ficheiro
    escreveFile ( final ) ;
    
    free(final) ;

    clock_t tfim = clock();

    float ttime ;
    ttime = ((double)(tfim - tinicio)) / CLOCKS_PER_SEC * 1000 ;

    printf ("Inês Vicente, a93269, Tomás Francisco, a93193 MIEI/CD, 3-jan-2021\nMódulo: t (cálculo dos códigos dos símbolos)\nNúmero de blocos: %lld\nTamanho dos blocos analisados no ficheiro de símbolos: 57444/1620 bytes\nTempo de execução do módulo (milissegundos): %f\nFicheiro gerado: exemplo.txt.rle.cod\n", B , ttime ) ;
    
    //return cod;
    
}


int main() {
    char * ff = "bbb.zip.freq" ;

    moduleTMain ( ff ) ;

    return 0 ;
}
