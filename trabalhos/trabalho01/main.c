#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "fila.h"

#define FALSE 0
#define TRUE 1

#define QTD_PROCESSOS 3
#define QTD_MAX_IO 10
#define QTD_MAX_PC 10

typedef struct processo {
    int id;
    pid_t pid;
    int pc;
    int pcFinal;
    int momentosIo[QTD_MAX_IO];
    int qtdIos;
    int ioAtual;
} Processo;

int tempo = 0;
int contagensProcessosFinalizados = 0;
Processo processos[QTD_PROCESSOS];
Processo *processoExecutando = NULL;

Fila *filaBloqueados;
Fila *filaProntos;

static int processoPodeContinuar = FALSE;

static void trataSigcont(int signo) {
    (void)signo;
    processoPodeContinuar = TRUE;
}

void interControllerSim(int kernel) {
    int segundos = 0;
    while (1) {
        sleep(1);
        segundos++;
        if (kill(kernel, 0) == -1) {
            break;
        }
        
        kill(kernel, SIGUSR1);

        if (segundos % 3 == 0) {
            kill(kernel, SIGUSR2);
        }
    }
}

void executaProcesso(Processo *processo){
    int indiceIo = processo->ioAtual;
    processoPodeContinuar = FALSE;
    int pcLocal = processo->pc;

    signal(SIGCONT, trataSigcont);

    while (1) {
        while (!processoPodeContinuar) pause();
        processoPodeContinuar = FALSE;

        if (pcLocal >= processo->pcFinal || pcLocal >= QTD_MAX_PC) break;

        if (indiceIo < processo->qtdIos && pcLocal == processo->momentosIo[indiceIo]) {
            printf(">>> [A%d] PC=%d -> Syscall de I/O acionada\n", processo->id, pcLocal);
            fflush(stdout);
            indiceIo++;
            processo->ioAtual = indiceIo;
            pcLocal++;
            kill(getppid(), SIGTTIN);
            raise(SIGSTOP);
            continue;
        }

        printf("[A%d] | EXECUTANDO\n", processo->id);
        fflush(stdout);
        pcLocal++;
        raise(SIGSTOP);
    }

    processo->pc = pcLocal;
    if (pcLocal >= QTD_MAX_PC) {
        printf("\n>>> [A%d] ATINGIU O LIMITE MÁXIMO DO SISTEMA (%d) E FOI ENCERRADO.\n", processo->id, QTD_MAX_PC);
    } else {
        printf("\n>>> [A%d] CHEGOU AO SEU PC FINAL (%d) E TERMINOU.\n", processo->id, processo->pcFinal);
    }
    fflush(stdout);
    exit(0);
}

void criaProcesso(int id, Processo *processo, int pcFinal, int *momentosIo, int qtdIos) {
    processo->id = id;

    processo->pid = fork();

    processo->pc = 0;

    processo->pcFinal = pcFinal;

    processo->qtdIos = qtdIos;

    processo->ioAtual = 0;

    for(int i = 0; i < qtdIos; i++){
        processo->momentosIo[i] = momentosIo[i];
    }

    if(processo->pid == 0){
        executaProcesso(processo);
        exit(0);
    }

    printf("[Tempo %ds] Kernel: Criou processo A%d PID=%d | PC=%d\n", tempo, processo->id, processo->pid, processo->pc);
    fflush(stdout);
    
    enfileiraFila(filaProntos, processo);
}

void finalizaTimeSlice(int sinal) {
    tempo++;
    if (processoExecutando == NULL) {
        return;
    }
    Processo *processoAtual = processoExecutando;
    int status = waitpid(processoAtual->pid, NULL, WNOHANG);
    
    if (status > 0) {
        contagensProcessosFinalizados++;
        printf("\n----------------------------\n");
        printf("[Tempo %ds] Kernel: [A%d] já havia finalizado (reap)\n", tempo, processoAtual->id);
        printf("----------------------------\n");
    } else {
        processoAtual->pc++;
        kill(processoAtual->pid, SIGSTOP);
        enfileiraFila(filaProntos, processoAtual);
        processoExecutando = NULL;
    }

    if (contagensProcessosFinalizados >= QTD_PROCESSOS) {
        printf("\n=== Kernel: Todos os processos finalizaram. Encerrando kernel ===\n");
        fflush(stdout);
        exit(0);
    }

    if (!filaEhVazia(filaProntos)) {
        Processo *proximoProcesso = desenfileiraFila(filaProntos);
        processoExecutando = proximoProcesso;
        printf("----------------------------\n");
        printf("[Tempo %ds] Kernel: Escalando -> A%d | PC=%d\n", tempo, proximoProcesso->id, proximoProcesso->pc);
        fflush(stdout);
        kill(proximoProcesso->pid, SIGCONT);
    }
}

void bloqueiaProcesso(int sinal) {
    if (processoExecutando == NULL) return;

    Processo *processoAtual = processoExecutando;
    processoAtual->pc++;
    printf("\n=== Kernel: [A%d] realizou Syscall ===\n", processoAtual->id);
    printf("PC=%d | Movido para fila de bloqueados\n", processoAtual->pc);
    enfileiraFila(filaBloqueados, processoAtual);
    processoExecutando = NULL;

    if (!filaEhVazia(filaProntos)) {
        Processo *proximoProcesso = desenfileiraFila(filaProntos);
        processoExecutando = proximoProcesso;
        printf("\n=== Kernel: Escalando [A%d] após bloqueio ===\n", proximoProcesso->id);
        printf("Restaurando contexto: PC=%d\n", proximoProcesso->pc);
        kill(proximoProcesso->pid, SIGCONT);
    }
}

void trataSigchld(int sinal) {
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        contagensProcessosFinalizados++;
        fflush(stdout);
        if(contagensProcessosFinalizados >= QTD_PROCESSOS){
            printf("----------------------------\n");
            printf("\n=== Kernel: Todos os processos finalizaram. Encerrando kernel. ===\n");
            fflush(stdout);
            exit(0);
        }
    }
}

void liberaIO(int sinal) {
    if (filaEhVazia(filaBloqueados)) return;

    Processo *processoLiberado = desenfileiraFila(filaBloqueados);

    printf("\n----------------------------\n");
    printf("[Tempo %ds] Kernel: I/O concluído -> A%d | PC=%d\n", tempo, processoLiberado->id, processoLiberado->pc);
    printf("----------------------------\n");
    enfileiraFila(filaProntos, processoLiberado);

    if (processoExecutando == NULL) {
        Processo *proximo = desenfileiraFila(filaProntos);
        if (proximo) {
            processoExecutando = proximo;
            printf("[Tempo %ds] Kernel: Escalando (apos IO) -> A%d | PC=%d\n", tempo, proximo->id, proximo->pc);
            printf("----------------------------\n");
            fflush(stdout);
            kill(proximo->pid, SIGCONT);
        }
    }
}

int main(void) {
    filaBloqueados = criaFila();
    filaProntos = criaFila();
    
    pid_t kernel = fork();

    if(kernel == 0){
        int iosA1[] = {};
        int iosA2[] = {};
        int iosA3[] = {};

        printf("\n=== Kernel: Simulação iniciada ===\n");
        printf("----------------------------\n\n");

        criaProcesso(1, &processos[0], 5, iosA1, sizeof(iosA1)/sizeof(iosA1[0]));
        criaProcesso(2, &processos[1], 5, iosA2, sizeof(iosA2)/sizeof(iosA2[0]));
        criaProcesso(3, &processos[2], 5, iosA3, sizeof(iosA3)/sizeof(iosA3[0]));

        printf("\n----------------------------\n");

        if (!filaEhVazia(filaProntos)) {
            Processo *primeiroProcesso = desenfileiraFila(filaProntos);
            processoExecutando = primeiroProcesso;
            printf("[Tempo %ds] -> A%d | PC=%d\n", tempo, primeiroProcesso->id, primeiroProcesso->pc);
            fflush(stdout);
            kill(primeiroProcesso->pid, SIGCONT);
        }

        signal(SIGUSR1, finalizaTimeSlice);
        signal(SIGUSR2, liberaIO);
        signal(SIGTTIN, bloqueiaProcesso);

        signal(SIGTTOU, bloqueiaProcesso);
        signal(SIGCHLD, trataSigchld);

        signal(SIGCHLD, handle_sigchld);

        while(1){
            pause();
        }
        exit(0);
    }

    pid_t pidControlador = fork();

    if (pidControlador == 0) {
        interControllerSim(kernel);

        exit(0);
    }

    waitpid(kernel, NULL, 0);
    waitpid(pidControlador, NULL, 0);
    
    return 0;
}
