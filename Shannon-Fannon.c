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
*/

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

const char * detectfreq (char * freq) {

    char *buffer = NULL;
    size_t size = 0;
    int i,j,arroba = 0;

/* Open your_file in read-only mode */
    FILE *fp = fopen(freq, "r");

/* Get the buffer size */
    fseek(fp, 0, SEEK_END); /* Go to end of file */
    size = ftell(fp); /* How many bytes did we pass ? */

/* Set position of stream to the beginning */
    rewind(fp);
    fseek(fp,3,SEEK_SET);
/* Allocate the buffer (no need to initialize it with calloc) */
    buffer = malloc((size + 1) * sizeof(*buffer)); /* size + 1 byte for the \0 */

/* Read the file into the buffer */
    fread(buffer, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */

/* NULL-terminate the buffer */
    buffer[size] = '\0';


    buffer = changeData ( buffer , size ) ;

/* Print it ! */
    printf("%s", buffer);

    fclose(fp);

    return buffer;
}

void main () {
    char *freq = "freq.txt";
    const char * arr = detectfreq (freq);
}
