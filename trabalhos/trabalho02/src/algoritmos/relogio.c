#include "simulador.h"

int relogio(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    static int ponteiro = 0;

    while (memoriaFisica[ponteiro].referenciada == 1) {
        memoriaFisica[ponteiro].referenciada = 0;
        ponteiro = (ponteiro + 1) % quantidadeDeQuadros;
    }

    int vitima = ponteiro;
    ponteiro = (ponteiro + 1) % quantidadeDeQuadros;

    return vitima;
}
