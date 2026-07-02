// Danielle Guimarães Cruz de Oliveira - 2320336 | LRU
// Lucas Manoel Martins de Souza - 2320715 | CLOCK e OTIMO
// Matheus da Silva Oliveira - 2320452 | NRU

#include "simulador.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: ./sim-virtual <algoritmo> <arquivo.log> <tamPaginaKB> <tamMemoriaMB>\n");
        return 1;
    }

    char *algoritmo = argv[1];
    char *arquivo = argv[2];
    int tamanhoDaPagina = atoi(argv[3]);
    int tamanhoDaMemoria = atoi(argv[4]);

    unsigned long pageFaultsTotal = 0;
    unsigned long paginasEscritasTotal = 0;

    int resultado = simular(algoritmo, arquivo, tamanhoDaPagina, tamanhoDaMemoria,
                            &pageFaultsTotal, &paginasEscritasTotal);

    if (resultado != 0) {
        return resultado;
    }

    imprimeResumo(algoritmo, arquivo, tamanhoDaMemoria, tamanhoDaPagina,
                  pageFaultsTotal, paginasEscritasTotal);

    return 0;
}

