#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "fila.h"

#define QNTPROCESSOS 3
#define QNTMAXIO 10
#define QNTPCMAX 10

typedef struct Processo {
    int id;
    pid_t pid;
    int pc;
    int pcFinal;
    int momentosIo[QNTMAXIO];
    int qtdIos;
    int ioAtual;
} Processo;

int tempo = 0;
volatile sig_atomic_t finishedCount = 0;

Processo processos[QNTPROCESSOS];
Processo *processo_executando = NULL;

Fila *fila_bloqueados;
Fila *fila_prontos;

void interControllerSim(int kernel){

    int segundos = 0;

    while(1){

        sleep(1);

        segundos++;

        if(kill(kernel, 0) == -1){
            if(errno == ESRCH) break;
        }

        kill(kernel, SIGUSR1);

        if(segundos % 3 == 0){
            kill(kernel, SIGUSR2);
        }
    }
}

void executaProcesso(Processo *processo){

    int ioIndex = processo->ioAtual;

    volatile sig_atomic_t cont_flag = 0;

    int local_pc = processo->pc;

    void sigcont_handler(int s){
        (void)s;
        cont_flag = 1;
    }

    signal(SIGCONT, sigcont_handler);

    while(1){

        while(!cont_flag) pause();

        cont_flag = 0;

        if(local_pc >= processo->pcFinal ||
           local_pc >= QNTPCMAX){
            break;
        }

        if(ioIndex < processo->qtdIos &&
           local_pc == processo->momentosIo[ioIndex]){

            printf("\n>>> [A%d] PC=%d -> Syscall de I/O acionada\n",
                   processo->id,
                   local_pc);

            fflush(stdout);

            ioIndex++;

            processo->ioAtual = ioIndex;

            local_pc++;

            kill(getppid(), SIGTTIN);

            raise(SIGSTOP);

            continue;
        }

        printf("[A%d] | EXECUTANDO | PC=%d\n",
               processo->id,
               local_pc);

        fflush(stdout);

        local_pc++;

        raise(SIGSTOP);
    }

    processo->pc = local_pc;

    if(local_pc >= QNTPCMAX){

        printf("\n>>> [A%d] ATINGIU O LIMITE MÁXIMO DO SISTEMA (%d) E FOI ENCERRADO.\n",
               processo->id,
               QNTPCMAX);

    } else {

        printf("\n>>> [A%d] CHEGOU AO SEU PC FINAL (%d) E TERMINOU.\n",
               processo->id,
               processo->pcFinal);
    }

    fflush(stdout);

    exit(0);
}

void criaProcesso(int id,
                  Processo *processo,
                  int pcFinal,
                  int *momentosIo,
                  int qtdIos){

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

    printf("[Tempo %ds] Kernel: Criou processo A%d PID=%d\n",
           tempo,
           processo->id,
           processo->pid);

    fflush(stdout);

    fila_enfileirar(fila_prontos, processo);
}

void finalizaTimeSlice(int sinal){

    tempo++;

    if(processo_executando == NULL){
        return;
    }

    Processo *processoAtual = processo_executando;

    int status =
        waitpid(processoAtual->pid,
                NULL,
                WNOHANG);

    if(status > 0){

        finishedCount++;

        printf("\n----------------------------\n");

        printf("[Tempo %ds] Kernel: [A%d] já havia finalizado\n",
               tempo,
               processoAtual->id);

        printf("----------------------------\n");

    } else {

        kill(processoAtual->pid, SIGSTOP);

        fila_enfileirar(fila_prontos,
                        processoAtual);
    }

    processo_executando = NULL;

    if(finishedCount >= QNTPROCESSOS){

        printf("\n=== Kernel: Todos os processos finalizaram ===\n");

        fflush(stdout);

        exit(0);
    }

    if(!fila_vazia(fila_prontos)){

        Processo *proximoProcesso =
            fila_desenfileirar(fila_prontos);

        processo_executando = proximoProcesso;

        printf("----------------------------\n");

        printf("[Tempo %ds] Kernel: Escalando -> A%d\n",
               tempo,
               proximoProcesso->id);

        fflush(stdout);

        kill(proximoProcesso->pid,
             SIGCONT);
    }
}

void bloqueiaProcesso(int sinal){

    if(processo_executando == NULL)
        return;

    Processo *processoAtual =
        processo_executando;

    printf("=== Kernel: [A%d] realizou Syscall ===\n",
           processoAtual->id);

    printf("Movido para fila de bloqueados\n\n");

    fila_enfileirar(fila_bloqueados,
                    processoAtual);

    processo_executando = NULL;

    if(!fila_vazia(fila_prontos)){

        Processo *proximoProcesso =
            fila_desenfileirar(fila_prontos);

        processo_executando =
            proximoProcesso;
        
        printf("----------------------------");
        printf("\n=== Kernel: Escalando [A%d] após bloqueio ===\n",
               proximoProcesso->id);

        fflush(stdout);

        kill(proximoProcesso->pid,
             SIGCONT);
    }
}

void handle_sigchld(int sinal){

    int status;

    pid_t pid;

    while((pid = waitpid(-1,
                         &status,
                         WNOHANG)) > 0){

        finishedCount++;

        if(finishedCount >= QNTPROCESSOS){

            printf("----------------------------\n");

            printf("\n=== Kernel: Todos os processos finalizaram. Encerrando kernel. ===\n");

            fflush(stdout);

            exit(0);
        }
    }
}

void finalizaIO(int sinal){

    if(fila_vazia(fila_bloqueados))
        return;

    Processo *processoLiberado =
        fila_desenfileirar(fila_bloqueados);

    printf("----------------------------\n\n");

    printf("[Tempo %ds] Kernel: I/O concluído -> A%d\n\n",
           tempo,
           processoLiberado->id);

    // printf("----------------------------\n");

    fila_enfileirar(fila_prontos,
                    processoLiberado);

    if(processo_executando == NULL){

        Processo *proximo =
            fila_desenfileirar(fila_prontos);

        if(proximo){

            processo_executando = proximo;

            printf("[Tempo %ds] Kernel: Escalando (apos IO) -> A%d\n",
                   tempo,
                   proximo->id);

            printf("----------------------------\n");

            fflush(stdout);

            kill(proximo->pid,
                 SIGCONT);
        }
    }
}

int main(){

    fila_bloqueados = fila_criar();

    fila_prontos = fila_criar();

    pid_t kernel = fork();

    if(kernel == 0){

        int iosA1[] = {};

        int iosA2[] = {};

        int iosA3[] = {};

        printf("\n=== Kernel: Simulação iniciada ===\n");

        printf("----------------------------\n\n");

        criaProcesso(1, &processos[0], 3, iosA1, sizeof(iosA1)/sizeof(iosA1[0]));

        criaProcesso(2,&processos[1],3, iosA2, sizeof(iosA2)/sizeof(iosA2[0]));

        criaProcesso(3, &processos[2], 3, iosA3, sizeof(iosA3)/sizeof(iosA3[0]));
        

        printf("\n----------------------------\n");

        if(!fila_vazia(fila_prontos)){

            Processo *primeiroProcesso =
                fila_desenfileirar(fila_prontos);

            processo_executando =
                primeiroProcesso;

            printf("[Tempo %ds] Kernel: Escalando -> A%d\n",
                   tempo,
                   primeiroProcesso->id);

            fflush(stdout);

            kill(primeiroProcesso->pid,
                 SIGCONT);
        }

        signal(SIGUSR1, finalizaTimeSlice);

        signal(SIGUSR2, finalizaIO);

        signal(SIGTTIN, bloqueiaProcesso);

        signal(SIGTTOU, bloqueiaProcesso);

        signal(SIGCHLD, handle_sigchld);

        while(1){
            pause();
        }

        exit(0);
    }

    pid_t interController = fork();

    if(interController == 0){

        interControllerSim(kernel);

        exit(0);
    }

    waitpid(kernel, NULL, 0);

    waitpid(interController, NULL, 0);

    return 0;
}