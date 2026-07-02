#ifndef MEMORIA_H
#define MEMORIA_H

#include "tipos.h"

int calculaQuantidadeDeQuadros(int tamanhoDaMemoria, int tamanhoDaPagina);
void inicializaMemoriaFisica(Quadro memoriaFisica[], int quantidadeDeQuadros);
Quadro *criaMemoriaFisica(int quantidadeDeQuadros);
int encontraQuadroVazio(Quadro memoriaFisica[], int quantidadeDeQuadros);
void resetaBitsReferenciaNRU(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quantidadeDeQuadros);

unsigned int calculaPagina(unsigned int enderecoVirtual, int tamanhoDaPagina);
void inicializaTabelaDePaginas(Pagina tabelaDePaginas[]);
void carregaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int pagina, char operacao);
void atualizaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], unsigned int pagina, char operacao);
void substituiPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int novaPagina, char operacao);

int validaTamanhoDaPagina(int tamanhoDaPagina);
int validaTamanhoDaMemoria(int tamanhoDaMemoria);
int validaAlgoritmos(char *algoritmo);

#endif
