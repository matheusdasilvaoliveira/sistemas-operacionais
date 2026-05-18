#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define QNTPROCESSOS 3

typedef struct{
    int pid;
    int pc;
} Processo;

int processoAtual = 0;
Processo processos[QNTPROCESSOS];



GQueue *filaBloqueados;
GQueue *filaProntos;

void interControllerSim(int kernel){
    int segundos;
    while(1){
        sleep(1);
        segundos++;
        
        kill(kernel, SIGUSR1);// significa que acabou o timeslice 

        if(segundos % 3 == 0){ 
            kill(kernel, SIGUSR2); // significa que acabou o IO
        }
    }
}


void executaProcesso(int id, int pc){
    while(1){
        printf("[A%d], em execução, pc = %d\n", id, pc);
        if(pc == 5 ){
            kill(getppid(), SIGTTIN);
        }
        pc++;
        
        sleep(1);
    }
}

void criaProcesso(int id, Processo *processo) {

    processo->pid = fork();
    processo->pc = 0;

    if(processo->pid == 0){
        executaProcesso(id, processo->pc);
        exit(0);
    }
    
    kill(processo->pid, SIGSTOP); 
    g_queue_push_tail(filaProntos, processo);
}


// tratadores de sinais 
void finalizaTimeSlice(int sinal){
    Processo processoAtual = g_queue_peek_head(filaProntos);

    kill(g_queue_pop_head(filaProntos), SIGSTOP);
    g_queue_push_tail(filaProntos, processoAtual);
    kill(g_queue_peek_head(filaProntos), SIGCONT);

}

void bloqueiaProcesso(int sinal){
    Processo processoAtual = g_queue_peek_head(filaProntos);

    kill(g_queue_pop_head(filaProntos), SIGSTOP);
    g_queue_push_tail(filaBloqueados, processoAtual);
}

void finalizaIO(int sinal){
    Processo processoAtual = g_queue_peek_head(filaBloqueados);
    g_queue_pop_head(filaBloqueados);
    g_queue_push_tail(filaProntos, processoAtual);
}

// principal
int main(){
    filaBloqueados = g_queue_new();
    filaProntos = g_queue_new();
    
    pid_t kernel = fork();

    if(kernel == 0){

        for(int i = 0; i<QNTPROCESSOS;i++){
            criaProcesso((i + 1), &processos[i]);

        }
        if(!g_queue_is_empty(filaProntos)){
            Processo primeiroProcesso=(*Processo) g_queue_peek_head(filaProntos);
            kill(primeiroProcesso->pid, SIGCONT);

        }

        signal(SIGUSR1, finalizaTimeSlice);
        signal(SIGUSR2, finalizaIO);
        signal(SIGTTIN, bloqueiaProcesso);
        signal(SIGTTOU, bloqueiaProcesso);

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

    
    
    return(1);
}


