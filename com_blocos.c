//Criado dia 3 dezembro


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void imprime_bloco (FILE *fp, int num_numeros)
{
		int i;
		int freq[num_numeros];
		for (i = 0; i < num_numeros-1; i++)
		{
			fscanf(fp, "%d;", &freq[i]);
		}
		fscanf(fp, "%d", &freq[i]);
		//print das frequências
		for (i = 0; i < num_numeros; i++) printf("\n Freq simb %d: %d ", i, freq[i]);
}
//Guarda os códigos a partir de um ficheiro binário
int * ler_bloco_ficheiro (FILE *fp, int valoresLidos)
{
    //[Endereços das codificações de cada char]  a codificação do 'c' freq[48] 
    int *freq[valoresLidos];
    freq[valoresLidos] = malloc (valoresLidos * (sizeof (int*)));  //Codificação dos 256 valores
    int pos_atual=0;  //posição atual no array das codificações
    int i;    
//    for (i = 0; i < size; i++) freq[i] = -1;
    int c = fgetc(fp);
    while(c != EOF && c != 64 && pos_atual < valoresLidos) //64 = @ in ASCII
        {
        if (c != 59) {   // 59 == ;

                            int comp_cod = ftell(fp); //Guarda posição atual
                            for (i = comp_cod; c == 48 || c == 49; i++) c = fgetc(fp);  //Conta qual o comprimento do código 
                            fseek( fp, comp_cod, SEEK_SET ); //Volta a meter o apontador antes, no início do código
                            comp_cod = i - comp_cod; //Tamanho da codificação
                            int *bitsTalvez = malloc( (1+comp_cod) * sizeof(int));  //Depois tens de fazer free()
                            //O array não precisa de ser char, porque só guarda 0's e 1's
                            //Guarda no array bitsTalvez a codificação
                            for (i=0; c == 48 || c == 49 ; i++) { 
                                                                        c = fgetc(fp);
                                                                        (bitsTalvez[i]) = c;
                                                                        }
                            (bitsTalvez[i]) = '\0';
                            (freq[pos_atual]) = bitsTalvez;



                        }
        else { 
                (freq[pos_atual] = NULL);
             }
                c = fgetc(fp);
        pos_atual++; 
        }
    return *freq;
}
//Esta main abre o ficheiro e guarda um array com a frequência de cada bloco, 1 de cada vez
int main () {
	//char c[15];
	FILE *fp;
    int valoresLidos = 256;
    int tam_bloc;
    //char *c;
	if ((fp = fopen("aaa.txt.cod", "r+")) == NULL) {
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
            ler_bloco_ficheiro(fp, 256);

            num_blocos--;
        }

        
         }
    fclose(fp);

	return 0;
    }

