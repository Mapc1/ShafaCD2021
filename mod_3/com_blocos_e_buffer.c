//Criado dia 3 dezembro


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void imprime_bloco (int valoresLidos, char ** freq, char * fim) {
	int carater_atual;
    int ii;
	for (carater_atual = 0; (freq[carater_atual]) != fim; carater_atual++) {
        printf ("carater de imprime %d é: ", carater_atual);
        if (freq[carater_atual] == NULL)  printf ("NADA \n");
        else { 
            printf("\n imprime else \n ");
            char *bitsTalvez= freq[carater_atual];
            for (ii = 0; bitsTalvez[ii] != '\0'; ii++) printf("%c  |  ", bitsTalvez[ii]); //Problema aqui!
            printf("\n"); 
 	    }
    }
}

//Guarda os códigos a partir de um ficheiro ASCII
char ** ler_bloco_ficheiro (char * buffer, int valoresLidos, char * fim) {
    //[Endereços das codificações de cada char]  a codificação do 'c' freq[0] 
    char **freq = malloc ((valoresLidos+1)* (sizeof (char*)));  //Endereços das codificações dos 256 valores
    int pos_atual_freq =0;  //posição atual no array das codificações
    int pos_atual_bufo = 0;
    int i;    
    //for (i = 0; i < size; i++) freq[i] = -1;
    char c = buffer[pos_atual_bufo];
    //Depois alterar para ser só diferente de '@
    while((c == ';' || c == '0' || c == '1')  && pos_atual_freq < valoresLidos){ //64 = @ in ASCII
        //printf ("\n %d \n ", pos_atual);
        printf ("\n carater de leitura %d é: %c", pos_atual_freq, c);
        if (c != ';') {   // 59 == ;                   
            int comp_cod = pos_atual_bufo; //Guarda posição atual
            for (i = pos_atual_bufo; c == '0' || c == '1'; i++) c = buffer[i]; //Conta qual o comprimento do código 
            comp_cod = i - pos_atual_bufo; //Tamanho da codificação
            freq[pos_atual_freq] = malloc( comp_cod * sizeof(char));
            char bitsTalvez[comp_cod];  //Depois tens de fazer free()
            //O array não precisa de ser char, porque só guarda 0's e 1's
            //Guarda no array bitsTalvez a codificação
            //  printf(" \n Posição atual do array %d \n", pos_atual);
            c = buffer[pos_atual_bufo];
            for (i=0; c == '0' || c == '1' ; i++) { 
                (bitsTalvez[i]) = c;
                pos_atual_bufo++;
                c = buffer[pos_atual_bufo];
            }
            (bitsTalvez[i]) = '\0';
            printf (" -_- ");
            for (i = 0; bitsTalvez[i] == '0' || bitsTalvez[i] == '1'; i++) printf("%c", bitsTalvez[i]); printf(" <- bits \n" );
            strcpy(freq[pos_atual_freq], bitsTalvez); 
            // (freq[pos_atual]) = bitsTalvez;
            //    c = fgetc(fp); //Ignorar o ; a seguir ao binário
            i = 9; //Apaga esta linha, é só para para no break


        }
        else (freq[pos_atual_freq] = NULL);
        pos_atual_bufo++;
        c = buffer[pos_atual_bufo];
        pos_atual_freq++; 
    }
    freq[pos_atual_freq] = fim ;
                          
    return freq;
}

// Função que cria o .rle.shaf (ESTÁ SUPER INCOMPLETA!!)  (Falta ver pq é q o file .shaf n fica como o do sor e fazer para as repetições)
void criaFIleRle(char **freq, char *buffer, char *fim, int tam) {
    FILE *rle = fopen ("aaa.txt.rle.shaf", "wb"); //criar o file
    int i = 0;
    char zero = '0';
    while (i < 100 ) { // este 100 é apenas para teste, mudar depois
            if (freq[buffer[i]] != NULL) fwrite (freq[buffer[i]],1, 1, rle); // para já ele só passa para binário os valores n repetidos mais de 4 vezes
            i++;
    }
}

//Função que guarda num buffer o ficheiro .rle
void guardaRLE (char **freq, char fim) {
    int i = 0;
    printf ("\n\n\n\n"); //apenas para separar dos outros testes
    FILE *rle = fopen ("aaa.txt.rle", "rb+");
    if ((rle == NULL)) printf("Error! opening file");
    fseek(rle, 0L, SEEK_END); //ir ao fim do ficheiro para ver o seu tamanho
    int tam = ftell(rle);
    fseek(rle, 0L, SEEK_SET); //voltar ao ínicio
    char buffer[tam];
    fgets (buffer, sizeof (buffer), rle); //já que o buffer é do tamanho do file, podemos pô-lo todo de uma vez (mt provavelmente vamos mudar depois)
    printf ("%u %u %u", buffer[0],buffer[1], buffer[2]); //apenas um teste
    fclose(rle); 
    printf ("\n"); 
    criaFIleRle(freq, buffer, &fim, tam);
}


//Se for o último bloco, isto podia ser diminuido
int ** ler_PontoCod (FILE *fp, int valoresLidos) {   
    int tam = valoresLidos + valoresLidos*valoresLidos;
    char buffer[tam];
    fread(&buffer, sizeof(char), tam, fp);
    //Valor estúpido para controlar fim do array
    char fim = '?';
    char **freq = ler_bloco_ficheiro(buffer, valoresLidos, &fim);
    imprime_bloco(valoresLidos, freq, &fim);
    guardaRLE(freq, fim); //basta pôr isto em comentário para n testar a minha função
}





//Esta main abre o ficheiro e guarda um array com a frequência de cada bloco, 1 de cada vez
int main () {
	//char c[15];
	FILE *fp;
    int valoresLidos = 257;
    int tam_bloc;
    int *freq[valoresLidos];
    //char *c;
	if ((fp = fopen("aaa.txt.cod", "r+")) == NULL)  printf("Error! opening file");         // Program exits if file pointer returns NULL.
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
        while (num_blocos > 0) {
            printf("\n Bloco atual (ao contrário): %d \n ", num_blocos);
            //informação para ler o ficheiro original
            fscanf(fp, "%d@", &tam_bloc);
            printf("\n Tamanho bloco: %d \n ", tam_bloc);
            ler_PontoCod(fp, valoresLidos);
            num_blocos--;
        }

        
    }
    //leRLE (fp, valoresLidos);
    fclose(fp);

	return 0;
}