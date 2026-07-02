#include "saida.h"
#include <stdio.h>

void imprimeResumo(const char *algoritmo, const char *arquivo, int tamanhoDaMemoria,
                   int tamanhoDaPagina, unsigned long pageFaultsTotal,
                   unsigned long paginasEscritasTotal) {
    printf("Executando o simulador...\n");
    printf("Arquivo de entrada: %s\n", arquivo);
    printf("Tamanho da memoria fisica: %d MB\n", tamanhoDaMemoria);
    printf("Tamanho das paginas: %d KB\n", tamanhoDaPagina);
    printf("Algoritmo de substituicao: %s\n", algoritmo);
    printf("Numero de Faltas de Paginas: %lu\n", pageFaultsTotal);
    printf("Numero de Paginas Escritas: %lu\n", paginasEscritasTotal);
}
