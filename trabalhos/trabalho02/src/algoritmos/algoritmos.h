#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "tipos.h"

int lru(Quadro memoriaFisica[], int quantidadeDeQuadros);
int relogio(Quadro memoriaFisica[], int quantidadeDeQuadros);
int otimo(Quadro memoriaFisica[], int quantidadeDeQuadros, unsigned int paginas[], int totalAcessos, int indiceAtual);
int nru(Quadro memoriaFisica[], int quantidadeDeQuadros);

#endif
