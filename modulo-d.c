#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shafa.h"
#include "modulo-d.h"

void readRLECode(FILE *fpRLE, char *symbol, char *repetitions){
  fread(symbol, 1, 1, fpRLE);
  fread(repetitions, 1, 1, fpRLE);
}

void decodeRLE(FILE *fpRLE, FILE *out){
  int i = 0, bytesWritten = 1;
  char repetitions;
  char symbol, buffer[BUFFSIZE] = "\0";

  while(bytesWritten){
    bytesWritten = fread(&symbol, 1, 1, fpRLE);
    if (bytesWritten){
      repetitions = 1;
      if(symbol == 0)
        readRLECode(fpRLE, &symbol, &repetitions);
      for(; repetitions > 0 && i < BUFFSIZE; i++, repetitions--)
        buffer[i] = symbol;
      if(i >= BUFFSIZE){
        writeFile(out, buffer, i);
        i = 0;
        if(repetitions > 0){
          for(; repetitions > 0 || i > BUFFSIZE; i++, repetitions--)
            buffer[i] = symbol;
        }
      }
    }
  }
  writeFile(out, buffer, i);
}

void readSection(FILE *fp, char *str){
  int i;
  for(i = 0; str[i-1] != '@'; i++)
    fread(str+i, 1, 1, fp);
  str[i-1] = '\0';
}

void initBlock(BlockData *block) {
    block->blockNum = 0;
    block->blockSize = 0;
    block->codes = malloc(sizeof(ABin));
    block->codes->left = NULL;
    block->codes->right = NULL;
    block->next = NULL;
}

ABin *createNode(){
  ABin *ret = malloc(sizeof(ABin));
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}

void getCodes(FILE *fpCOD, BlockData *block){
  ABin **tmp;
  unsigned char bit, gotCode = 0;

  block->codes = createNode();

  for(int symbol = 0; symbol < NSIMBOLOS; symbol++){
    tmp = &(block->codes);
    fread(&bit, 1, 1, fpCOD);
    while(bit != ';' && bit != '@'){
      if(bit == '1'){
          tmp = &((*tmp)->right);
          if(!(*tmp))
            *tmp = createNode();
      }
      else {
          tmp = &((*tmp)->left);
          if(!(*tmp))
              *tmp = createNode();
      }
      fread(&bit, 1, 1, fpCOD);
      gotCode = 1;
    }
    if (gotCode){
        (*tmp)->c = (char) symbol;
        gotCode = 0;
    }
  }
}

BlockData *readCOD(FILE *fpCOD){
  BlockData **tmp, *block;
  char buffer[BUFFSIZE];
  size_t totalBlocks;
  Precomp compression;

  fseek(fpCOD, 1, SEEK_SET);
  readSection(fpCOD, buffer);
  compression = (int) buffer[0];

  readSection(fpCOD, buffer);
  totalBlocks = strtol(buffer, NULL,10);

  tmp = &block;
  for(int i = 0; i < totalBlocks; i++){
      (*tmp) = malloc(sizeof(BlockData));
      initBlock(*tmp);
      (*tmp)->compress = compression;
      (*tmp)->blockNum = i;
      readSection(fpCOD, buffer);
      (*tmp)->blockSize = strtol(buffer, NULL, 10);
      getCodes(fpCOD, *tmp);
      tmp = &((*tmp)->next);
  }
  return block;
}

//recebe um ficheiro do tipoo .cod e retorna 1 se tiver sido ultilizada a compressão rle 0 caso contrário
int itsrle (FILE *f){
    char s[3];
    fgets(s,3,f);
    int r=1;
    if(s[1]=='N') r=0;
  return r;
}

//função que calcula o número de blocos do ficheiro
int nblock (FILE *f) {
int r,i=3,a=0;
char c;
fseek( f,3, SEEK_SET);
c = fgetc(f);
while( c != '@' ){
          c = fgetc( f );
          i++;          
}
char s[i-2];
fseek( f,3, SEEK_SET);
fgets(s,i,f);
r=atoi(s);
return r;
}
//função que calcula o numero de digitos de um numero
int ndigit (int a) {
int r=0;
while(a!=0) {
r++;
a=a/10;
}
return r;
}

// função que recebe um ficheiro do tipo .cod e retorna uma string com as tabelas do codigo sf de todos os blocos
char *toarray (FILE *f) {
int n,i=0;
n=ndigit(nblock(f));
n=n+4;
char c; char *s;
fseek( f,n, SEEK_SET);
c = fgetc(f);
while( c != '@' ){
          c = fgetc( f );
          n++;          
}
n++;
fseek( f,n, SEEK_SET);
while( c != EOF ){
          c = fgetc( f );
          i++;          
}
i--;
fseek( f,n, SEEK_SET);
s=malloc(i*sizeof(char));
fgets(s,i,f);
return s;
}

//função que recebe uma string e devolve uma string com os elementos entre i e f
char *partstring (char *s,int i,int f) {
   char *r;
   int a,t;
   t=f-i;
   r=malloc(t*sizeof(char));
   for(a=0;a<t && i<=f;a++) {
   r[a]=s[i];
   i++;
   }
  return r;
} 

//função que recebe uma string e devolve a tabela SF do primeiro bloco
int *tabelaSF (char *a,int N) {
  int i,ind=0,last=0,end=0;
  int *r;
  char *aux;
  r=malloc(256*sizeof(int));
  for(i=0;i<N;i++){
    while(a[i]!=';'&& a[i]!='\0') {
      end++; 
      i++; }    
    if(last==end++){
      r[ind]=-1;
      }
    else {
      aux=partstring(a,last,end);
    r[ind]=atoi(aux);
    }
    ind++;
    last=end;
    }
  return r;
}

//função que recebe uma string e retira a cabeça da string
void retirahead (char s[]) {
int i=0;
while (s[i+1]!='\0') {
  s[i]=s[i+1];
  i++;
}
}

//função que recebe uma string e retira os N primeiros elementos da string
void retira (char s[],int N) {
int i;
for(i=0;i<N;i++) {
retirahead (s);
}
}

//função que cria a matriz com (nº blocos) linhas e 256 colunas(nº simbolos)
int **matrizSF (FILE *f) {
int t;
t=nblock(f);
int **matriz;
int i,j;
int *aux;
char *a;
for (int i = 0; i < t; ++i) {matriz[i] = malloc (256 * sizeof (int));}
a=toarray(f);
while(i<t) {
for(i=0;a[i]!='@';i++){}
aux=tabelaSF(a,i-1);
retira(a,i);
for(i=0;a[i]!='@';i++){}
retira(a,i);
for(j=0;j<256;j++){
matriz[i][j]=aux[j];
}
}
return matriz;
}

void decodeShafa(FILE *fpSF, FILE *fpCOD, FILE *fout){
  int **tab;
  tab=matrizSF(fpCOD);
}

void moduleDMain(Options *opts){
  char *foutName, *fin2;
  FILE *fout, *fin1 = fopen(opts->fileIN, "rb");
  BlockData *blockInfo;

  switch(opts->optD){
    case 's':
      if (!opts->fileOUT)
        foutName = removeSufix(opts->fileIN, ".shafa");
      else foutName = opts->fileOUT;
      fout = fopen(foutName, "wb");
      //decodeShafa();
      blockInfo = readCOD(fin1);
      fseek(fin1, 10, SEEK_SET);
      if(!opts->fileOUT) free(foutName);
      fclose(fout);
      break;
    case 'r':
      if(!opts->fileOUT)
        opts->fileOUT = removeSufix(opts->fileIN, ".rle");
      fout = fopen(foutName, "wb");
      decodeRLE(fin1, fout);
      fclose(fout);
      break;
    //case '\0': decodeNormal();
    default: fprintf(stderr, "Erro!! Opção esta opção não existe!\n");
  }
  fclose(fin1);
}
