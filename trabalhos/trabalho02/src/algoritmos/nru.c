#include "simulador.h"

int nru(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    int quadroVitima = -1;
    int menorClasse = 4;

    for (int numQuadro = 0; numQuadro < quantidadeDeQuadros; numQuadro++) {
        int flagReferenciada = memoriaFisica[numQuadro].referenciada;
        int flagModificada = memoriaFisica[numQuadro].modificada;
        int classeAtual;

        if (flagReferenciada == 0 && flagModificada == 0) {
            classeAtual = 0;
        } else if (flagReferenciada == 0 && flagModificada == 1) {
            classeAtual = 1;
        } else if (flagReferenciada == 1 && flagModificada == 0) {
            classeAtual = 2;
        } else {
            classeAtual = 3;
        }

        if (classeAtual < menorClasse) {
            menorClasse = classeAtual;
            quadroVitima = numQuadro;

            if (menorClasse == 0) {
                break;
            }
        }
    }

    return quadroVitima;
}
