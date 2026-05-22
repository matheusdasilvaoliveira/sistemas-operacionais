#include "fila.h"
#include <stdio.h>
#include <stdlib.h>

struct FilaNode {
    Processo *processo;
    struct FilaNode *next;
};

struct Fila {
    struct FilaNode *head;
    struct FilaNode *tail;
    int length;
};

Fila *fila_criar(void) {
    Fila *fila = malloc(sizeof(Fila));
    if (!fila) {
        perror("Erro ao alocar memória para a fila");
        exit(EXIT_FAILURE);
    }
    fila->head = NULL;
    fila->tail = NULL;
    fila->length = 0;
    return fila;
}

int fila_vazia(const Fila *fila) {
    return fila->length == 0;
}

void fila_enfileirar(Fila *fila, Processo *processo) {
    struct FilaNode *node = malloc(sizeof(struct FilaNode));
    if (!node) {
        perror("Erro ao alocar nó da fila");
        exit(EXIT_FAILURE);
    }
    node->processo = processo;
    node->next = NULL;

    if (fila_vazia(fila)) {
        fila->head = node;
        fila->tail = node;
    } else {
        fila->tail->next = node;
        fila->tail = node;
    }
    fila->length++;
}

Processo *fila_desenfileirar(Fila *fila) {
    if (fila_vazia(fila)) {
        return NULL;
    }
    struct FilaNode *node = fila->head;
    Processo *processo = node->processo;
    fila->head = node->next;
    if (fila->head == NULL) {
        fila->tail = NULL;
    }
    free(node);
    fila->length--;
    return processo;
}

Processo *fila_frente(const Fila *fila) {
    if (fila_vazia(fila)) {
        return NULL;
    }
    return fila->head->processo;
}
