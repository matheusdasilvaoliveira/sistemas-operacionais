#ifndef FILA_H
#define FILA_H

typedef struct processo Processo;
typedef struct fila Fila;

Fila *criaFila(void);
int filaEhVazia(const Fila *fila);
void enfileiraFila(Fila *fila, Processo *processo);
Processo *desenfileiraFila(Fila *fila);
Processo *primeiroProcessoNaFila(const Fila *fila);

#endif
