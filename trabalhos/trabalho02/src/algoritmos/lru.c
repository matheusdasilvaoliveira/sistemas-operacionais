#include "simulador.h"

int lru(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    int quadroLRU = 0;

    for (int i = 1; i < quantidadeDeQuadros; i++) {
        if (memoriaFisica[i].tempo < memoriaFisica[quadroLRU].tempo) {
            quadroLRU = i;
        }
    }

    return quadroLRU;
}
