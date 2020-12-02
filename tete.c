#include <stdio.h>
#include <stdlib.h>


int main () {
	//char c[15];
	FILE *fp;
	char *c;
	if ((fp = fopen("teste.txt", "r+")) == NULL) {
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
		char num_blocos;	
		fscanf(fp, "@%c@", &num_blocos);
		int numi_blocos;
		numi_blocos =  (int)num_blocos - 48;
		printf("\n Número de elementos: %d", numi_blocos);
		printf(" A partir daqui bloqueia");
		//Ler um array de ints com ;
		int i;
		int freq[num_blocos];
		for (i = 0; i < num_blocos-1; i++)
		{
			fscanf(fp, "%d;", &freq[i-1]);
		}
		fscanf(fp, "%d", &freq[i-1]);
		for (i = 0; i < num_blocos; i++) printf("Freq simb %d: %d", i, freq[i]);
	}
		/*Ler um array de char
		c = malloc(2* num_blocos* sizeof(char));
		fscanf(fp, "%s", c);
		printf("%d \n", num_blocos);
        for (int i = 0; i < num_blocos  && c[i] != '\0'; i++) 
						printf ("%c", c[i]);	
						/*
	}
	/*printf("Escolhe um número");
	int num;
	scanf("%d", &num);
	fprintf(fp, "%d", num);*/
	fclose(fp);
	free(c);
	return 0;
    }
