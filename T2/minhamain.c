#include <stdlib.h>
#include "compilalinb.h"

#define TAM_CODIGO 1024

int main(void) {
  unsigned char codigo[TAM_CODIGO];
  FILE *myfp;
  funcp f; 
  //char *nome_arquivo = argv[1];
  int retorno;

  if ((myfp = fopen ("arq.txt", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }

  f = compilaLinB(myfp, codigo);

  retorno = (*f)(10,20); /* se a função não recebe parametros, são descartados */
  
  printf("resultado =  %d\n",retorno); 

  }