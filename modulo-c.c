#ifdef __linux__
  

//Criado dia 3 dezembro

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include "shafa.h"
#include "modulo-c.h"
//#include "Modulo_f/funcoesAuxiliares.c"

//#include "fsize.h"
/*
struct thread_data{
    unsigned char *buffer_cod;


    int valoresASCII;
    char * fim_cod;
    unsigned char *buffer;
    int tam;
    int tam_Shaf;
    unsigned char *arr_final;
    int bloco_atual;
} ;
*/
char *nomeFicheiroExtensao1(const char *nomeFicheiro, const char *extensao) {
    size_t length = strlen(nomeFicheiro) + strlen(extensao) + 1;
    char *concat = malloc(sizeof(char) * length);
    if (!concat) return NULL; // Malloc error
    snprintf(concat, length, "%s%s", nomeFicheiro, extensao);

    return concat;
}
//Converte [01110101101] num valor entre 0 e 257
char converte_Para_Byte (char *byte) {
    unsigned char devolve = 0;
    int teste = 0;
    int pos_atual= 7 ; 
    int expoenteDois = 1;
    while(pos_atual >= 0) {
        devolve += expoenteDois * (byte[pos_atual]-48);
        teste += expoenteDois * (byte[pos_atual]-48);
        expoenteDois *= 2;
        pos_atual--;
    }    
    return devolve;
}

void converteListaArray (char* array_final, char *result) {
    //pos_atual no array_final
    int i;
    char byte [8];
    int k;
    int tam = strlen (result);
    int pos_atual = 0;
    for (i = 0; i< tam;i++) {
        for (k = 0; k < 8 && i < tam ; k++, i++) byte [k] = result [i]; 
        for (; k < 8; k++) byte[k] = '0'; 
         array_final[pos_atual] = converte_Para_Byte(byte);
         pos_atual++;
         if (i == tam) return;
    }
}

void Limpa_Array_Arrays(char **array, int valoresLidos) {
    int i;
    for (i = 0; i <valoresLidos; ++i) {
        free(array[i]);
    }
    free(array);
}

//Guarda os códigos a partir de um ficheiro ASCII
char ** ler_bloco_ficheiro_cod (char * buffer, char * fim) {
    //[Endereços das codificações de cada char]  a codificação do 'c' cod[0] 
    char **cod = malloc ((VALORES_ASCII+1)* (sizeof (char*)));  //Endereços das codificações dos 256 valores
    int pos_atual_cod =0;  //posição atual no array das codificações
    int pos_atual_bufo = 0;
    int i;    
    char c = buffer[pos_atual_bufo];
    //Depois alterar para ser só diferente de '@
    while((c == ';' || c == '0' || c == '1')  && pos_atual_cod < VALORES_ASCII){ //64 = @ in ASCII
        if (c != ';') {   // 59 == ;                   
            int comp_cod = pos_atual_bufo; //Guarda posição atual
            for (i = pos_atual_bufo; c == '0' || c == '1'; i++) c = buffer[i]; //Conta qual o comprimento do código 
            comp_cod = i - pos_atual_bufo; //Tamanho da codificação
            cod[pos_atual_cod] = malloc( comp_cod * sizeof(char));
            char bitsTalvez[comp_cod];  //Depois tens de fazer free()
            //O array não precisa de ser char, porque só guarda 0's e 1's
            //Guarda no array bitsTalvez a codificação
            c = buffer[pos_atual_bufo];
            for (i=0; c == '0' || c == '1' ; i++) { 
                (bitsTalvez[i]) = c;
                pos_atual_bufo++;
                c = buffer[pos_atual_bufo];
            }
            (bitsTalvez[i]) = '\0';
            strcpy(cod[pos_atual_cod], bitsTalvez); 
            i = 9; //Apaga esta linha, é só para para no break
        }
        else (cod[pos_atual_cod] = NULL);
        pos_atual_bufo++;
        c = buffer[pos_atual_bufo];
        pos_atual_cod++; 
    }
    cod[pos_atual_cod] = fim ;
 //   *comp_cod_bloc += pos_atual_bufo+1;                     
    return cod;
}


int potencia(int base, int exp)
{
    
    int result = 1;
    
    while (exp != 0) {
        result *= base;
        --exp;
    }
    return result;
}
 int ordena_cod(char * buffer, int * pos_buffer, int * tamanhos, int * bloc_atual, char * result)
{
    int pos_at = *pos_buffer;
    int tamanho_result = pos_at;
    //char * copia;
//    result = malloc((257+257*257 )*  sizeof(char));
          //  cod[pos_atual_cod] = malloc( comp_cod * sizeof(char));
    int copia_at = 0;
    pos_at++;
    char c = buffer[pos_at];
    tamanhos[*bloc_atual] = 0;
    int i;
    for (i = pos_at; c >= 48 && c <= 57 ; i++)   c = buffer[i];
    i = i-2 - pos_at;

    c = buffer[pos_at];
    while (c >= 48 && c <= 57) { 
                (tamanhos[*bloc_atual]) += (c-48) * potencia(10,i); pos_at++; c = buffer[pos_at];i--; }
    pos_at++;
    c = buffer[pos_at];
    while (c != '@' && c != '\0')
    {
        result[copia_at] = c;
        copia_at++;
        pos_at++;
    c = buffer[pos_at];
    }
    result[copia_at] = '\0';
    *pos_buffer = pos_at;
    return pos_at-tamanho_result;
            
}

void  strcpyApontadores(char * copias, char *temp)
{
    int i;
    for (i = 0; (temp[i]) != '\0'; i++ ) 
    copias[i] = temp[i]; 
} 

char ** le_PontoCod ( int * tamanhos, FILE * fp, int num_bloc, unsigned long long int tam_ficheiro)
{ 
    char **copias = malloc(num_bloc * sizeof (char*));
    int pos_buffer = 0;
    int bloc_atual = 0; 
    int tam_cada_bloco;
    tam_ficheiro++;
   char buffer[tam_ficheiro];
   char *temp;
    temp = malloc((VALORES_ASCII+VALORES_ASCII*VALORES_ASCII )*  sizeof(char));
            fread(&buffer, sizeof(char), tam_ficheiro, fp);
   while (bloc_atual < num_bloc)
        {
             tam_cada_bloco = ordena_cod (buffer, &pos_buffer, tamanhos, &bloc_atual, temp);
            copias[bloc_atual] = malloc( tam_cada_bloco * sizeof(char));
            strcpyApontadores(copias[bloc_atual],temp); 
            bloc_atual++;
        }
       free(temp);
        return copias;
        
}
/*
//Se for o último bloco, isto podia ser diminuido
char ** usa_PontoCod (char * buffer, int valoresLidos) {    //Este fp é .cod
    //int tam = valoresLidos + valoresLidos*valoresLidos;
    //char buffer[tam];
    //fread(&buffer, sizeof(char), tam, fp);
    //Valor estúpido para controlar fim do array
    char fim = '?';
    char **cod = ler_bloco_ficheiro_cod(buffer, valoresLidos, &fim);
   // imprime_bloco(valoresLidos, cod, &fim);
//    guardaRLE(cod, fim); //basta pôr isto em comentário para n testar a minha função
    return cod;
}
*/
void *PontoShafa (void * testeT){
    struct thread_data *teste;
    //teste = malloc (sizeof (struct thread_data));
    teste = (struct thread_data *) testeT;
    char * buffer_cod =  teste->buffer_cod; 
   // int valoresLidos = teste->valoresASCII;
    char * fim = teste->fim_cod;
    int tam_buffer = teste->tam_Ori;
    char *buffer = teste -> buffer;
    char **cod = ler_bloco_ficheiro_cod(buffer_cod, fim);
    free(buffer_cod);
    int bloco_atual = teste -> bloco_atual;
    char *array_final = teste -> arr_final;
    char *result = malloc (8 * sizeof (char));
    tam_buffer--;
    //char *new_arr = NULL;
    int k = 0, o = 0, i, j;
    unsigned char devolve;
    char *bitsTalvez = malloc (256 * sizeof(char));
    for (i = 0; i <= teste -> tam_Ori - 1  ; i++) {
        bitsTalvez = cod[teste->buffer[i]];
    for (j = 0; bitsTalvez[j] != '\0'; j ++ , k++) 
        {
        if (k == 8) {
            devolve = converte_Para_Byte (result);
            teste -> arr_final [o] = devolve;
            o++;
            for (; k > 0 ; k--) result [k-1] = '0';
            k = 0;
        }
        result [k] = bitsTalvez[j];
        }
    }
    //if (k != 1) {
     //   printf ("AQUIIIIII!!!!   k -> %d   i --> %d\n", k, i);
        for (; k < 8; k++) result [k] = '0';
        devolve = converte_Para_Byte (result);
        teste -> arr_final [o] = devolve;
    //}
    o++;
    teste -> tam_Shaf = o;
    testeT = teste;
    printf("\nTamanho antes/depois & taxa de compressão (bloco %d): %d/ %d / %d", bloco_atual, tam_buffer,o, tam_buffer/o);
    pthread_exit(NULL);
    Limpa_Array_Arrays(cod, VALORES_ASCII);
    free(buffer_cod);
    free(buffer);

}
unsigned long long int tamanho_ficheiro(char * nome)
{
    /* unsigned long long total;
    long long n_blocks;
    unsigned long size_of_last_block, block_size;
    block_size = 2048;
    int num_blocos = fsize(NULL, nome, &block_size, &size_of_last_block);//tam_fich(fp);
    total = (n_blocks-1) * block_size + size_of_last_block;
    printf("Aqui\n \n \n");
    return total;*/ 
    int tam = 0;
    //char c; 
    FILE* fp = fopen(nome, "rb");
    while (!feof(fp))
        {
      c = fgetc(fp);
      tam++;
    }
      rewind(fp);
      return tam;
}


int moduleCMain (Options * opts, FileCreated **list) {
    clock_t inicio = clock();
	FILE *fp, *fptr, *fpOrigi;
    //int valoresLidos = 257;
    int tam_bloc;
    int *cod[VALORES_ASCII];
   // int i;
    char *ficheiro_cod = nomeFicheiroExtensao1(opts->fileIN, ".cod");
	 if ((fptr = fopen(nomeFicheiroExtensao1(opts->fileIN, ".shaf"),"wb")) == NULL){
       printf("Error! opening file");
       return 0;
    }
    addFilesCreated(list, opts->fileIN);
  if ((fpOrigi = fopen(opts->fileIN ,"r")) == NULL){      printf("Error! opening file");       return 0;    }
  //if ((fpOrigi = fopen("aaa.txt","r")) == NULL){   printf("Error! opening file");      return 0;    }
    else {
		//Ler tipo de ficheiro
        char tipo;
        unsigned long long int tam_ficheiro_cod = tamanho_ficheiro(ficheiro_cod);
        if ((fp = fopen(ficheiro_cod, "r+")) == NULL)  printf("Error! opening file");         // Program exits if file pointer returns NULL.
        int bloco_atual = 0;
		fscanf(fp, "@%c", &tipo);
		int num_blocos;	
		fscanf(fp, "@%d", &num_blocos);
        fprintf(fptr, "@%d", num_blocos);
        // .cod
        infoTerminalINI(num_blocos);
        int  tamanhos_cod[num_blocos];
        int num_blocos_MT = 8;
        int num_blocos_MT_fim = num_blocos_MT;
        char **arr_cods =le_PontoCod (tamanhos_cod, fp, num_blocos, tam_ficheiro_cod);
        char **arr_Oris= malloc(num_blocos_MT * sizeof (char*));
        struct thread_data teste[num_blocos];
        pthread_t threads[8];
//            int comp_cod_bloc = ftell(fp;

        while (bloco_atual < num_blocos) {
                       int blocoMTAtual ;
                   
           // if (num_blocos_MT * (bloco_atual+1)  > num_blocos) num_blocos_MT_fim = num_blocos-(bloco_atual*num_blocos_MT);
                       for (blocoMTAtual = 0; blocoMTAtual < 8; blocoMTAtual++)
                            { 
                            int tamanhoBlocoMTAtual = tamanhos_cod[num_blocos_MT*bloco_atual+blocoMTAtual];
                            unsigned char *buffer= malloc (tamanhoBlocoMTAtual * sizeof(char));
                            fread(buffer, sizeof(char), tamanhoBlocoMTAtual, fpOrigi);
                            buffer[tamanhoBlocoMTAtual] = '\0';
                            char *arr_final = malloc (tamanhoBlocoMTAtual * sizeof(char));
                            
                            teste[blocoMTAtual].buffer = buffer;
                            teste[blocoMTAtual].tam_Ori = tamanhoBlocoMTAtual;
                            teste[blocoMTAtual].arr_final = arr_final;
                            arr_Oris[blocoMTAtual] = buffer;
                            }
                    //Ciclo da thread

                   for (blocoMTAtual = 0; blocoMTAtual < 8; blocoMTAtual++){
                        int bloc_MEMO = num_blocos_MT*bloco_atual+blocoMTAtual;
                    teste[blocoMTAtual].buffer_cod = arr_cods[bloc_MEMO];
                    //teste[blocoMTAtual].valoresASCII = valoresLidos;
                    char fim = '?';
                    teste[blocoMTAtual].fim_cod = &fim;

                    teste[blocoMTAtual].bloco_atual = blocoMTAtual;
                    int rc;
                    //PontoShafa(arr_Oris[blocoMTAtual], fptr, cod0, tamanhos_cod[bloc_MEMO], bloco_atual);
                    rc = pthread_create(&threads[blocoMTAtual], NULL, PontoShafa, (void *) &teste[blocoMTAtual]);
                    if (rc) {
                        printf("ERROR; return code from pthread_create() is %d\n", rc);
                        exit(-1);
                    }
                    pthread_join (threads[blocoMTAtual], NULL);
                    //printf ("DEPOIS_BUFFER\n");
                    fprintf (fptr, "@%d@", teste[blocoMTAtual].tam_Shaf);
                    //unsigned char arr_t = *teste.arr_final;
                    int len = strlen (teste[blocoMTAtual].arr_final);
                    //printf ("DEPOIS_FPRINTF\n");
                    //int len = strlen (teste.arr_final);
                    //printf ("ARRAY AGAIN -> %s\n", teste[blocoMTAtual].arr_final);
                    fwrite (teste[blocoMTAtual].arr_final, teste[blocoMTAtual].tam_Shaf, sizeof(char), fptr);
                  
                    //printf ("DEPOIS_FWRITE\n");
                   }
                    bloco_atual+= num_blocos_MT-1;
                    pthread_exit (NULL);
        }
    }
    clock_t fim = clock();
    printf("Tempo de execução do módulo: %f milisegundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000);
    return 0;
}
void infoTerminalINI(int num_blocs) {
    
    printf("Diogo Pires, a93308, Jorge Melo, a93286, MIEI/CD, ");
    data();
  //  printf("Módulo: c (Módulo para codificação do ficheiro original/RLE)\n");
  //  printf("Número de blocos: %d  \n", num_blocos);
        
}


#endif
