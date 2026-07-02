#include "memoria.h"

int calculaQuantidadeDeQuadros(int tamanhoDaMemoria, int tamanhoDaPagina) {
    return (tamanhoDaMemoria * 1024) / tamanhoDaPagina;
}

void inicializaMemoriaFisica(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    for (int i = 0; i < quantidadeDeQuadros; i++) {
        memoriaFisica[i].pagina = -1;
        memoriaFisica[i].referenciada = 0;
        memoriaFisica[i].modificada = 0;
        memoriaFisica[i].tempo = 0;
    }
}

Quadro *criaMemoriaFisica(int quantidadeDeQuadros) {
    Quadro *memoriaFisica = malloc(quantidadeDeQuadros * sizeof(Quadro));
    if (memoriaFisica == NULL) {
        printf("Erro ao alocar memoria fisica.\n");
        return NULL;
    }

    inicializaMemoriaFisica(memoriaFisica, quantidadeDeQuadros);
    return memoriaFisica;
}

int encontraQuadroVazio(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    for (int i = 0; i < quantidadeDeQuadros; i++) {
        if (memoriaFisica[i].pagina == -1) {
            return i;
        }
    }
    return -1;
}

void resetaBitsReferenciaNRU(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quantidadeDeQuadros) {
    for (int numQuadro = 0; numQuadro < quantidadeDeQuadros; numQuadro++) {
        memoriaFisica[numQuadro].referenciada = 0;
        if (memoriaFisica[numQuadro].pagina != -1) {
            tabelaDePaginas[memoriaFisica[numQuadro].pagina].referenciada = 0;
        }
    }
}

unsigned int calculaPagina(unsigned int enderecoVirtual, int tamanhoDaPagina) {
    if (tamanhoDaPagina == 4) {
        return enderecoVirtual >> 12;
    }
    return enderecoVirtual >> 13;
}

void inicializaTabelaDePaginas(Pagina tabelaDePaginas[]) {
    for (int i = 0; i < MAX_PAGINAS; i++) {
        tabelaDePaginas[i].presente = 0;
        tabelaDePaginas[i].quadro = -1;
        tabelaDePaginas[i].referenciada = 0;
        tabelaDePaginas[i].modificada = 0;
    }
}

void carregaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int pagina, char operacao) {
    memoriaFisica[quadro].pagina = pagina;
    memoriaFisica[quadro].referenciada = 1;
    memoriaFisica[quadro].modificada = (operacao == 'W');

    tabelaDePaginas[pagina].presente = 1;
    tabelaDePaginas[pagina].quadro = quadro;
    tabelaDePaginas[pagina].referenciada = 1;
    tabelaDePaginas[pagina].modificada = (operacao == 'W');
}

void atualizaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], unsigned int pagina, char operacao) {
    int quadro = tabelaDePaginas[pagina].quadro;

    tabelaDePaginas[pagina].referenciada = 1;
    memoriaFisica[quadro].referenciada = 1;

    if (operacao == 'W') {
        tabelaDePaginas[pagina].modificada = 1;
        memoriaFisica[quadro].modificada = 1;
    }
}

void substituiPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int novaPagina, char operacao) {
    unsigned int paginaAntiga = memoriaFisica[quadro].pagina;

    tabelaDePaginas[paginaAntiga].presente = 0;
    tabelaDePaginas[paginaAntiga].quadro = -1;
    tabelaDePaginas[paginaAntiga].referenciada = 0;
    tabelaDePaginas[paginaAntiga].modificada = 0;

    carregaPagina(memoriaFisica, tabelaDePaginas, quadro, novaPagina, operacao);
}

int validaTamanhoDaPagina(int tamanhoDaPagina) {
    if (tamanhoDaPagina != 4 && tamanhoDaPagina != 8) {
        printf("Erro: tamanho da pagina deve ser 4 ou 8 KB.\n");
        return 0;
    }
    return 1;
}

int validaTamanhoDaMemoria(int tamanhoDaMemoria) {
    if (tamanhoDaMemoria != 1 && tamanhoDaMemoria != 2 && tamanhoDaMemoria != 4) {
        printf("Erro: memoria deve ser 1, 2 ou 4 MB.\n");
        return 0;
    }
    return 1;
}

int validaAlgoritmos(char *algoritmo) {
    if (strcmp(algoritmo, "LRU") != 0 && strcmp(algoritmo, "NRU") != 0 && strcmp(algoritmo, "CLOCK") != 0 && strcmp(algoritmo, "OTIMO") != 0) {
        printf("Algoritmo invalido.\n");
        return 0;
    }
    return 1;
}
