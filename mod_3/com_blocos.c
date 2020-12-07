//Criado dia 3 dezembro


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void imprime_bloco (int valoresLidos, int ** freq, int * fim)
{
		int carater_atual;
        int ii;
		for (carater_atual = 0; (freq[carater_atual]) != fim; carater_atual++)
		{
            printf ("carater de imprime %d é: ", carater_atual);
            if (freq[carater_atual] == NULL)  printf ("NADA \n");
            else { 
                printf("\n imprime else \n ");
                   int *bitsTalvez= freq[carater_atual];
                    for (ii = 0; bitsTalvez[ii] != '\0'; ii++) printf("%d  |  ", bitsTalvez[ii]); //Problema aqui!
                    printf("\n"); 
		        }
        }
}
//Guarda os códigos a partir de um ficheiro binário
int ** ler_bloco_ficheiro (FILE *fp, int valoresLidos, int * fim)
{
    //[Endereços das codificações de cada char]  a codificação do 'c' freq[48] 
    int **freq = malloc ((valoresLidos+1)* (sizeof (int*)));  //Codificação dos 256 valores
    int pos_atual=0;  //posição atual no array das codificações
    int i;    
//    for (i = 0; i < size; i++) freq[i] = -1;
    int c = fgetc(fp);
    while((c == 48 || c == 49 || c == 59)  && pos_atual < valoresLidos) //64 = @ in ASCII
        {
        //printf ("\n %d \n ", pos_atual);
        printf ("\n carater de leitura %d é: %c", pos_atual, c);
        if (c != 59) {   // 59 == ;
                            int comp_cod = ftell(fp); //Guarda posição atual
                            for (i = comp_cod; c == 48 || c == 49; i++) c = fgetc(fp);  //Conta qual o comprimento do código 
                            fseek( fp, comp_cod-1, SEEK_SET ); //Volta a meter o apontador antes, no início do código
                            comp_cod = i - comp_cod; //Tamanho da codificação
                            freq[pos_atual] = malloc( (1+comp_cod) * sizeof(int));
                            int bitsTalvez[comp_cod+1];  //Depois tens de fazer free()
                            //O array não precisa de ser char, porque só guarda 0's e 1's
                            //Guarda no array bitsTalvez a codificação
                          //  printf(" \n Posição atual do array %d \n", pos_atual);
                            c = fgetc(fp);
                            for (i=0; c == 48 || c == 49 ; i++) { 
                                                                        (bitsTalvez[i]) = c;
                                                                        c = fgetc(fp);
                                                                        }
                            (bitsTalvez[i]) = '\0';
                            for (i = 0; bitsTalvez[i] == 48 || bitsTalvez[i] == 49; i++) printf("%c", bitsTalvez[i]); printf(" <- bits \n" );
                            strcpy(freq[pos_atual], bitsTalvez);
                           // (freq[pos_atual]) = bitsTalvez;
                                        //    c = fgetc(fp); //Ignorar o ; a seguir ao binário
                            i = 9; //Apaga esta linha, é só para para no break


                        }
        else { 
                (freq[pos_atual] = NULL);
             }
                c = fgetc(fp);
        pos_atual++; 
        }
        freq[pos_atual] = fim ;
        fseek( fp, -1, SEEK_CUR ); //Se não come um carater
                            
    return freq;
}
//Esta main abre o ficheiro e guarda um array com a frequência de cada bloco, 1 de cada vez
int main () {
	//char c[15];
	FILE *fp;
    int valoresLidos = 80;
    int tam_bloc;
    int *freq[valoresLidos];
    //char *c;
	if ((fp = fopen("blocos.txt", "r+")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
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
        while (num_blocos > 0)
        {
            printf("\n Bloco atual (ao contrário): %d \n ", num_blocos);
            fscanf(fp, "%d@", &tam_bloc);
            printf("\n Tamanho bloco: %d \n ", tam_bloc);
            int fim = 9;
           int **freq = ler_bloco_ficheiro(fp, valoresLidos, &fim);
            imprime_bloco(valoresLidos, freq, &fim);

            num_blocos--;

        }

        
         }
    fclose(fp);

	return 0;
    }

