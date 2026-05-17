#include <stdio.h>
#include <stdlib.h>
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

typedef struct{
    int fila[6];
    int inicio;
    int fim;
    
} FilaIO;

void interControllerSim(int kernel){
    while(1){
        printf("[controller], em execução...\n");
        
        if(0){ // TODO: vai virá o tamanho da fila
            sleep(3);
            kill(kernel, SIGUSR2);
        }

        sleep(5);
        kill(kernel,SIGUSR1);
    }
}


void executaProcesso(int id, int pc){
    while(1){
        printf("[A%d], em execução, pc = %d\n", id, pc);
        pc++;
        
        // Tem que ter um pc maximo e em algum momento 
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
}

void escalonaProcesso(){
    
    kill(processos[processoAtual].pid, SIGSTOP);

    processoAtual = (processoAtual+1) % QNTPROCESSOS;

    kill(processos[processoAtual].pid, SIGCONT);
}

void trataTimeSlice(int sinal){
    escalonaProcesso(); 

}


void trataIO(int sinal){

}

int main(){

    pid_t kernel = fork();

    if(kernel == 0){

        for(int i = 0; i<QNTPROCESSOS;i++){
            criaProcesso((i + 1), &processos[i]);

        }

        kill(processos[0].pid, SIGCONT);

        signal(SIGUSR1, trataTimeSlice);
        signal(SIGUSR2, trataIO);

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


