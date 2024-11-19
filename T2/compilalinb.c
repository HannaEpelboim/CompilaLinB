/*
Erica Regnier 2211893 3WB
Hanna Epelboim Assunção 2310289 3WB
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "compilalinb.h"

typedef struct listaReferencia ListaReferencia;
struct listaReferencia {
    int destino;
    int indiceCodigo;
    ListaReferencia* prox;
};


static void error(const char* msg, int line) {
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}



int opera(char op, char var0, char var1, char var2, int idx0, int idx1, int idx2, int ind, unsigned char* codigo) {

    //var1 --> r10d
    if (var1 == '$') {
        codigo[ind++] = 0x49;

        //81 ou 83?
        codigo[ind++] = 0xc7;
        codigo[ind++] = 0xc2;

        unsigned char* p = (unsigned char*)&idx1;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;
    }
    else if (var1 == 'p') {
        codigo[ind++] = 0x49;
        codigo[ind++] = 0x89;
        if (idx1 == 1) {
            codigo[ind++] = 0xfa;

        }
        else if (idx1 == 2) {
            codigo[ind++] = 0xf2;
        }
    }
    else if (var1 == 'v') {
        codigo[ind++] = 0x4c;
        codigo[ind++] = 0x8b;

        if (idx1 == 1) { //-8(%rbp)
            codigo[ind++] = 0x55;
            codigo[ind++] = 0xf8;

        }
        else if (idx1 == 2) {
            codigo[ind++] = 0x55;
            codigo[ind++] = 0xf0;
        }
        else if (idx1 == 3) {
            codigo[ind++] = 0x55;
            codigo[ind++] = 0xe8;
        }
        else if (idx1 == 4) {
            codigo[ind++] = 0x55;
            codigo[ind++] = 0xe0;
        }
    }

    //var2 --> r11d
    if (var2 == '$') {
        codigo[ind++] = 0x49;
        //81 ou 83?
        codigo[ind++] = 0xc7;
        codigo[ind++] = 0xc3;
        unsigned char* p = (unsigned char*)&idx2;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;
        codigo[ind++] = *p++;

    }
    else if (var2 == 'p') {
        codigo[ind++] = 0x49;
        codigo[ind++] = 0x89;
        if (idx2 == 1) {
            codigo[ind++] = 0xfb;

        }
        else if (idx2 == 2) {
            codigo[ind++] = 0xf3;
        }
    }
    else if (var2 == 'v') {
        codigo[ind++] = 0x4c;
        codigo[ind++] = 0x8b;
        if (idx2 == 1) {
            codigo[ind++] = 0x5d;
            codigo[ind++] = 0xf8;
        }
        else if (idx2 == 2) {
            codigo[ind++] = 0x5d;
            codigo[ind++] = 0xf0;
        }
        else if (idx2 == 3) {
            codigo[ind++] = 0x5d;
            codigo[ind++] = 0xe8;
        }
        else if (idx2 == 4) {
            codigo[ind++] = 0x5d;
            codigo[ind++] = 0xe0;
        }
    }
    //operador  XXXl %r10d,%r11d

    if (op == '+') { //45 01 d3  
        codigo[ind++] = 0x45;
        codigo[ind++] = 0x01;
        codigo[ind++] = 0xd3;


    }
    else if (op == '-') {//subl %r11d,r10d   movq r10,r11
        codigo[ind++] = 0x45;
        codigo[ind++] = 0x29;
        codigo[ind++] = 0xda;

        codigo[ind++] = 0x4d;
        codigo[ind++] = 0x89;
        codigo[ind++] = 0xd3;
    }
    else if (op == '*') {//45 0f af da 
        codigo[ind++] = 0x45;
        codigo[ind++] = 0x0f;
        codigo[ind++] = 0xaf;
        codigo[ind++] = 0xda;
    }
    //
        //var0
    if (var0 == 'p') {

        codigo[ind++] = 0x4c;
        codigo[ind++] = 0x89;
        switch (idx0) {
        case 1: {
            //associar ele com edi

            codigo[ind++] = 0xdf;

            break;
        }
        case 2: {
            //associar ele com esi
            codigo[ind++] = 0xde;

            break;
        }
        default: printf("opcao invalida!\n");
        }
    }

    else if (var0 == 'v') {

        codigo[ind++] = 0x4c;
        codigo[ind++] = 0x89;
        codigo[ind++] = 0x5d;

        switch (idx0) {
        case 1: {
            codigo[ind++] = 0xf8;

            break;
        }
        case 2: {
            codigo[ind++] = 0xf0;

            break;
        }
        case 3: {
            codigo[ind++] = 0xe8;

            break;
        }
        case 4: {
            codigo[ind++] = 0xe0;

            break;
        }
        default: printf("opcao invalida!\n");
        }
    }

    return ind;

}

funcp compilaLinB(FILE* f, unsigned char codigo[]) {
    int line = 1;
    int  c;
    int ind = 0;
    /*Prologo*/
    /***************************************************************************/
    //pushq %rbp
    codigo[ind++] = 0x55;
    //movq% rsp, % rbp
    codigo[ind++] = 0x48;
    codigo[ind++] = 0x89;
    codigo[ind++] = 0xe5;

    //subq $32, % rsp
    codigo[ind++] = 0x48;
    codigo[ind++] = 0x83;
    codigo[ind++] = 0xec;
    codigo[ind++] = 0x20;

    /***************************************************************************/

    ListaReferencia* ant = (ListaReferencia*)malloc(sizeof(ListaReferencia));
    if (!ant) {
        fprintf(stderr, "lista nao criada.\n");
        exit(1);
    }
    ant->indiceCodigo = -1;
    ant->destino = -1;
    ant->prox = NULL;


    int linhas[50]; //vetor que guarda o ind de codigo que comeca a linha (obs ind = linha - 1)

    while ((c = fgetc(f)) != EOF) {
        linhas[line - 1] = ind;

        switch (c) {
        case 'r': { /* retorno */
            char c0;
            if (fscanf(f, "et%c", &c0) != 1)
                error("comando invalido", line);
            //printf("ret\n");
            //retornar:
            //valor de ret no rax
            //valor da prox func no rbp
            //leave ret

        //48 8b 45 f8          	mov    -0x8(%rbp),%rax
            codigo[ind++] = 0x48;
            codigo[ind++] = 0x8b;
            codigo[ind++] = 0x45;
            codigo[ind++] = 0xf8;
            codigo[ind++] = 0xc9;
            codigo[ind++] = 0xc3;
            //printf("cod: %x, pos:%d\n",codigo[ind-1],ind-1);
            break;
        }
        case 'v':
        case 'p': {  /* atribuicao */
            int idx0, idx1, idx2;
            char var0 = c, var1, var2, op;
            if (fscanf(f, "%d <= %c%d %c %c%d", &idx0, &var1, &idx1,
                &op, &var2, &idx2) != 6)
                error("comando invalido", line);
            //printf("%c%d = %c%d %c %c%d\n",
            //        var0, idx0, var1, idx1, op, var2, idx2);
            ind = opera(op, var0, var1, var2, idx0, idx1, idx2, ind, codigo);
            //printf("cod: %x, pos:%d\n",codigo[ind-1],ind-1);
            break;
        }
        case 'i': { /* desvio */ //'if' varp num
            char var0;
            int idx0, num;
            if (fscanf(f, "f %c%d %d", &var0, &idx0, &num) != 3)
                error("comando invalido", line);
            //printf("if %c%d %d\n", var0, idx0, num);
            //var0 --> r10d
/*********************************************************************************************************************************************/

            if (var0 == '$') {
                codigo[ind++] = 0x49;

                //81 ou 83?
                codigo[ind++] = 0xc7;
                codigo[ind++] = 0xc2;

                unsigned char* p = (unsigned char*)&idx0;
                codigo[ind++] = *p++;
                codigo[ind++] = *p++;
                codigo[ind++] = *p++;
                codigo[ind++] = *p++;
            }
            else if (var0 == 'p') {
                codigo[ind++] = 0x49;
                codigo[ind++] = 0x89;
                if (idx0 == 1) {
                    codigo[ind++] = 0xfa;

                }
                else if (idx0 == 2) {
                    codigo[ind++] = 0xf2;
                }
            }
            else if (var0 == 'v') {
                codigo[ind++] = 0x4c;
                codigo[ind++] = 0x8b;

                if (idx0 == 1) { //-8(%rbp)
                    codigo[ind++] = 0x55;
                    codigo[ind++] = 0xf8;

                }
                else if (idx0 == 2) {
                    codigo[ind++] = 0x55;
                    codigo[ind++] = 0xf0;
                }
                else if (idx0 == 3) {
                    codigo[ind++] = 0x55;
                    codigo[ind++] = 0xe8;
                }
                else if (idx0 == 4) {
                    codigo[ind++] = 0x55;
                    codigo[ind++] = 0xe0;
                }
            }
            /*********************************************************************************************************************************************/
                        //cmpl $0, %r10d
                        //41 83 fa 00
            codigo[ind++] = 0x41;
            codigo[ind++] = 0x83;
            codigo[ind++] = 0xfa;
            codigo[ind++] = 0x00;
            //jne 
            codigo[ind++] = 0x0f;
            codigo[ind++] = 0x85;
            //anota na lista:
            if (ant->indiceCodigo == -1) {

                // printf("ind1: %d\n",ind);

                ant->indiceCodigo = ind;
                // printf("ind1: %d\n",ant->indiceCodigo);

                ant->destino = num;
                ant->prox = NULL;
            }
            else {
                //printf("entre\n");
                ListaReferencia* p = (ListaReferencia*)malloc(sizeof(ListaReferencia));
                if (!p) {
                    fprintf(stderr, "lista nao criada.\n");
                    exit(1);
                }
                p->indiceCodigo = ind;
                // printf("ind2: %d\n",ind);
                // printf("ind2: %d\n",p->indiceCodigo);

                p->destino = num;
                ant->prox = p;
                p->prox = NULL;
            }

            codigo[ind++] = 0x00;
            codigo[ind++] = 0x00;
            codigo[ind++] = 0x00;
            codigo[ind++] = 0x00;
            //printf("%x ",codigo[ind]);




            break;
        }
        default: error("comando desconhecido", line);
        }
        line++;
        fscanf(f, " ");
    }
    //       for(int i = 0; i<50; i++){
    //     printf("%x ",codigo[i]);
    //   }
    //     putchar('\n');

    if (ant->indiceCodigo != -1) {
        ListaReferencia* primeirinho = ant;
        while (ant) {
            int indice = ant->indiceCodigo;
            // printf("%d\n",indice);
            // printf("%d\n",ant->indiceCodigo);

            //instrucao - rip

            int indiceDestino = linhas[ant->destino - 1];
            int indiceRip = indice + 4;
            int offset = indiceDestino - indiceRip;
            unsigned char* p = (unsigned char*)&offset;

            codigo[indice++] = *p++;
            // printf("cod: ");
            // printf("%x \n",codigo[indice]);

            codigo[indice++] = *p++;
            // printf("%x \n",codigo[indice]);

            codigo[indice++] = *p++;
            //printf("%x \n",codigo[indice]);

            codigo[indice++] = *p++;
            //printf("%x \n",codigo[indice]);

            ant = ant->prox;
        }
        while (primeirinho) {
            ant = primeirinho;
            primeirinho = primeirinho->prox;
            free(ant);
        }
    }

    /*Final*/
    /***************************************************************************/
    /***************************************************************************/

    return (funcp)codigo;
}