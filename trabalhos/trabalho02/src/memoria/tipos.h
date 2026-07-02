#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGINAS 1048576
#define ACESSOS_RESET_NRU 2000

extern unsigned tempoAtual;
extern unsigned long pageFaults;
extern unsigned long paginasEscritas;

typedef struct {
    int presente;
    int quadro;
    int referenciada;
    int modificada;
} Pagina;

typedef struct {
    int pagina;
    int referenciada;
    int modificada;
    unsigned tempo;
} Quadro;

#endif
