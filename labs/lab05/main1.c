#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define EVER ;;

void intHandler(int sinal);
void quitHandler(int sinal);

int main (void) {
    void (*p)(int); // ponteiro para função que recebe int como parâmetro
    p = signal(SIGINT, intHandler);
    printf("Endereco do manipulador anterior %p\n", p);
    p = signal(SIGQUIT, quitHandler);
    printf("Endereco do manipulador anterior %p\n", p);
    printf ("Ctrl-C desabilitado. Use Ctrl-\\ para terminar");
    for(EVER);
}

void intHandler(int sinal) {
    printf("\nVocê pressionou Ctrl-C (%d)", sinal);
    return;
}

void quitHandler(int sinal) {
    puts ("\nTerminando o processo...");
    exit (0);
}