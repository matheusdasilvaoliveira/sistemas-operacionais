#ifndef SAIDA_H
#define SAIDA_H

void imprimeResumo(const char *algoritmo, const char *arquivo, int tamanhoDaMemoria,
                   int tamanhoDaPagina, unsigned long pageFaultsTotal,
                   unsigned long paginasEscritasTotal);

#endif
