#include "fila.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    Processo *processo;
    struct node *proximo;
} Node;

typedef struct fila {
    Node *inicio;
    Node *fim;
    int tamanho;
} Fila;

Fila *criaFila(void) {
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    if (fila == NULL) {
        perror("malloc");
        exit(1);
    }
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;

    return fila;
}

int filaEhVazia(const Fila *fila) {
    return fila->tamanho == 0;
}

void enfileiraFila(Fila *fila, Processo *processo) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    
    node->processo = processo;
    node->proximo = NULL;

    if (filaEhVazia(fila)) {
        fila->inicio = node;
    } else {
        fila->fim->proximo = node;
    }
    fila->fim = node;
    fila->tamanho++;
}

Processo *desenfileiraFila(Fila *fila) {
    if (filaEhVazia(fila)) {
        return NULL;
    }
    
    Node *node = fila->inicio;
    Processo *processo = node->processo;
    fila->inicio = node->proximo;
    
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }
    
    free(node);
    fila->tamanho--;
    
    return processo;
}

Processo *primeiroProcessoNaFila(const Fila *fila) {
    if (filaEhVazia(fila)) {
        return NULL;
    }
    return fila->inicio->processo;
}
