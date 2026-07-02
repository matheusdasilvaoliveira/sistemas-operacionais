#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "tipos.h"
#include "memoria.h"
#include "algoritmos.h"
#include "arquivo.h"
#include "saida.h"

int simular(char *algoritmo, char *arquivo, int tamanhoDaPagina, int tamanhoDaMemoria,
            unsigned long *pageFaultsTotal, unsigned long *paginasEscritasTotal);

#endif
