#include <stdio.h>
#include <stdlib.h>

/*
int cal_melhor_divisao (int freq [],int i,int j) {
    int div = i,total = mindif = dif = soma(freq,i,j),g1 = 0;
    while (dif == mindif) {
        g1 = g1 + freq[div];
        dif = abs(2*g1 - total);
        if (dif < mindif) {
            div++;
            mindif = dif;
        }
        else {dif = mindif + 1;}
    }
    return (div - 1);
}

int soma (int freq [],int i,int j) {
    int sum = 0,index;
    for (index = i;index <= j;index++) sum += freq[index];
    return sum;
}

char * changeData ( char * buffer , int size ) {
  int i,j,arroba = 0,index = 0;
  char *test;
  test = malloc(size * sizeof(char));

  for (i = 0;buffer[i] != '\0';i++) {
    if ( ((arroba % 2) == 0) && (arroba != 0) ) {
      if (buffer[i] == '@') {
        arroba++;
        test[index] = ';';printf("%c",test[index]);
        index++;
      }
      else {
        test [index] = buffer[i];printf("%c",buffer[i]);
        index++;
      }
    }
    else {
      if (buffer[i] == '@') arroba++; 
    }
  }
  return test;
}
*/ 

char * detectfreq (char * freq) { // Função que lê o conteúdo do ficheiro e passa-o para um array de char(string).

    char *buffer = NULL; // Variável que guardará a string.
    size_t size = 0; // Variável que representa o tamanho para a string buffer.
    int i = 0; // Variável para dar print (remover quando for necessário).

    FILE *fp = fopen(freq, "r"); // Abrir o ficheiro para leitura.

    // As próximas duas linhas servem para a mandar o ficheiro para EOF.Isto serve para determinar o tamanho da nossa string
    // e o ftell serve para nos dizer quantos caracteres o nosso ficheiro possui para dar um tamanho à string.
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp); 

    rewind(fp); // O ficheiro volta para o SEEK_SET,ou seja,o inicio do ficheiro,para o voltar a ler.

    buffer = malloc((size + 1) * sizeof(*buffer)); // Usando o size,determinamos o tamanho da nossa string,ou então,buffer.Usamos para o malloc para alocar espaço e sizeof para determinar o tamanho de cada elemento do buffer.


    fread(buffer, size, 1, fp); // A partir do nosso file,iremos ler um 1 bloco de size bytes para cada incrementação de buffer.

    buffer[size] = '\0'; // Colocar o último elemento ('\0') no array,para indicar o final.

    // Imprimir a string(simples verificação).

    for (i = 0;buffer[i] != '\0';i++) {
        printf("%c",buffer[i]);
    }

    // Fechar o ficheiro.

    fclose(fp);

    //Devolver a string.

    return buffer;
}

FILE * writeFile (char * freq) { // Função que escreve uma string num ficheiro.

    FILE * fp;

    fp = fopen("Drag Test","w"); // Abrir o fichero para escrita(Neste caso também criamos o ficheiro se ele não existir.)

    fputs (freq,fp); // fputs é uma função que irá colocar o conteúdo de uma string no ficheiro,neste caso,a string freq para o ficheiro fp.

    fclose(fp); // Fechar o ficheiro fp.

    return (fp); // Retornar fp.
}

void main () {
    char *freq = "freq.txt";
    const char * arr = detectfreq (freq);
}
