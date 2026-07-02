#ifndef PAGINAS_H
#define PAGINAS_H

#include "tipos.h"

unsigned int calculaPagina(unsigned int enderecoVirtual, int tamanhoDaPagina);
void inicializaTabelaDePaginas(Pagina tabelaDePaginas[]);
void carregaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int pagina, char operacao);
void atualizaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], unsigned int pagina, char operacao);
void substituiPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int novaPagina, char operacao);

#endif
