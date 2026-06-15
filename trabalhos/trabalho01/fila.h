#ifndef FILA_H
#define FILA_H

typedef struct Processo Processo;
typedef struct Fila Fila;

Fila *fila_criar(void);
int fila_vazia(const Fila *fila);
void fila_enfileirar(Fila *fila, Processo *processo);
Processo *fila_desenfileirar(Fila *fila);
Processo *fila_frente(const Fila *fila);

#endif
