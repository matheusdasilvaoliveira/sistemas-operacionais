#include "simulador.h"

int otimo(Quadro memoriaFisica[], int quantidadeDeQuadros, unsigned int paginas[], int totalAcessos, int indiceAtual) {
    int vitima = 0;
    int maisLonge = -1;

    for (int q = 0; q < quantidadeDeQuadros; q++) {
        int proximoUso = totalAcessos;

        for (int j = indiceAtual + 1; j < totalAcessos; j++) {
            if (paginas[j] == (unsigned int) memoriaFisica[q].pagina) {
                proximoUso = j;
                break;
            }
        }

        if (proximoUso > maisLonge) {
            maisLonge = proximoUso;
            vitima = q;
        }
    }

    return vitima;
}
