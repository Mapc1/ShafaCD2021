//Criado dia 3 dezembro

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Converte [01110101101] num valor entre 0 e 257
char converteByteEstupido (char *byte) {
    char devolve = 0;
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
    for (i = 0; i< tam;) {
        for (k = 0; k < 8 && i < tam ; k++, i++) byte [k] = result [i]; 
        for (; k < 8; k++) byte[k] = '0'; 
         array_final[pos_atual] = converteByteEstupido(byte);
         pos_atual++;
         if (i == tam) return;
    }
}

//Guarda os códigos a partir de um ficheiro ASCII
char ** ler_bloco_ficheiro (char * buffer, int valoresLidos, char * fim, int * comp_cod_bloc) {
    //[Endereços das codificações de cada char]  a codificação do 'c' freq[0] 
    char **freq = malloc ((valoresLidos+1)* (sizeof (char*)));  //Endereços das codificações dos 256 valores
    int pos_atual_freq =0;  //posição atual no array das codificações
    int pos_atual_bufo = 0;
    int i;    
    char c = buffer[pos_atual_bufo];
    //Depois alterar para ser só diferente de '@
    while((c == ';' || c == '0' || c == '1')  && pos_atual_freq < valoresLidos){ //64 = @ in ASCII
        if (c != ';') {   // 59 == ;                   
            int comp_cod = pos_atual_bufo; //Guarda posição atual
            for (i = pos_atual_bufo; c == '0' || c == '1'; i++) c = buffer[i]; //Conta qual o comprimento do código 
            comp_cod = i - pos_atual_bufo; //Tamanho da codificação
            freq[pos_atual_freq] = malloc( comp_cod * sizeof(char));
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
            strcpy(freq[pos_atual_freq], bitsTalvez); 
            i = 9; //Apaga esta linha, é só para para no break
        }
        else (freq[pos_atual_freq] = NULL);
        pos_atual_bufo++;
        c = buffer[pos_atual_bufo];
        pos_atual_freq++; 
    }
    freq[pos_atual_freq] = fim ;
    *comp_cod_bloc += pos_atual_bufo+1;                     
    return freq;
}

//buffer contêm ficheiro .txt
//fptr .shaf, no fim para guardar o buffer

char *nomeFicheiroExtensao(char *nomeFicheiro, char *extensao) {
    size_t length = strlen(nomeFicheiro) + strlen(extensao) + 1;
    char *concat = malloc(sizeof(char) * length);
    if (!concat) return NULL; // Malloc error
    snprintf(concat, length, "%s%s", nomeFicheiro, extensao);
    return concat;
}

void PontoShafa (char* buffer, FILE *fptr, char **freq, int tam_buffer){
    int atual;
    char c;
    char *result = malloc (256 * sizeof (char));
    int size = 256;
    int resposta; 
    tam_buffer--;
    char *new_arr = NULL;
    printf ("AQUI\n");
    for (atual = 0; atual < tam_buffer; atual++) {
        if (atual > size-255) {
            result = realloc (result, 2*size);
            size *= 2;
        }
        result = nomeFicheiroExtensao (result, freq[buffer[atual]]);
        //strcat (result, freq[buffer[atual]]);
    }
    printf ("AQUI\n");
    printf ("%s\n", result);
    int tam;
    if (strlen (result) % 8 == 0) tam = strlen (result) / 8;
    else tam = (strlen (result) / 8) + 1;
    char array_final[atual];
    fprintf(fptr, "@%d@", tam);
    converteListaArray(array_final, result);
    fwrite(array_final , tam, sizeof(char) , fptr );
    printf("\n AAA Olá %s \n", array_final);    

}

//Se for o último bloco, isto podia ser diminuido
char ** ler_PontoCod (FILE *fp, int valoresLidos, int * comp_cod_bloc) {    //Este fp é .cod
    int tam = valoresLidos + valoresLidos*valoresLidos;
    char buffer[tam];
    fread(&buffer, sizeof(char), tam, fp);
    //Valor estúpido para controlar fim do array
    char fim = '?';
    char **freq = ler_bloco_ficheiro(buffer, valoresLidos, &fim, comp_cod_bloc);
    return freq;
}


//Esta main abre o ficheiro e guarda um array com a frequência de cada bloco, 1 de cada vez
int main () {
    clock_t inicio = clock();
	FILE *fp, *fptr, *fpOrigi;
    int valoresLidos = 257;
    int tam_bloc;
    int *freq[valoresLidos];
    int i;
	if ((fp = fopen("aaa.txt.cod", "r+")) == NULL)  printf("Error! opening file");         // Program exits if file pointer returns NULL.
    if ((fptr = fopen("aaaM.shaf","wb")) == NULL){
       printf("Error! opening file");
       return 0;
    }
    if ((fpOrigi = fopen("aaa.txt","r")) == NULL){
       printf("Error! opening file");
       return 0;
    }
    else {
		//Ler tipo de ficheiro
		char tipo;
		fscanf(fp, "@%c", &tipo);
		if (tipo == 'N') printf("Normal");
		else printf("Reles");
		//Número de blocos
		int num_blocos;	
		fscanf(fp, "@%d@", &num_blocos);
        printf("  número de blocos %d \n", num_blocos);
        //int num_bloc_temp = num_blocos;
        fprintf(fptr, "@%d", num_blocos);
        int comp_cod_bloc = ftell(fp);
        while (num_blocos > 0) {
            printf("\n Bloco atual (ao contrário): %d \n ", num_blocos);
            //informação para ler o ficheiro original
            fscanf(fp, "%d@", &tam_bloc);
            printf("\n Tamanho bloco: %d \n ", tam_bloc);
            //Count digits of tam_bloc
            int count = 0;
            int temp = tam_bloc;
            while (temp != 0) {
                temp /= 10;     // n = n/10
                ++count;
            }
            comp_cod_bloc += count+1; 
            char ** freq = ler_PontoCod(fp, valoresLidos, &comp_cod_bloc);
            num_blocos--;
            //Guarda ficheiro original, primeiro bloco
            char buffer [tam_bloc+1];
            fread(&buffer, sizeof(char), tam_bloc, fpOrigi);
            buffer[tam_bloc+1] = '\0';
            PontoShafa(buffer, fptr, freq, tam_bloc+1); 
            fseek(fp, comp_cod_bloc, SEEK_SET);
        }
    }
    fclose(fp);
    fclose(fptr);
    fclose(fpOrigi);
    clock_t fim = clock();
    printf("Tempo de execução do módulo: %f milisegundos\n", ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000);
	return 0;
}

