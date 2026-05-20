#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define QNTPROCESSOS 3
#define QNTMAXIO 10
#define QNTPCMAX 10

typedef struct{
    int id;
    pid_t pid;
    int pc; 
    int pcFinal;              
    int momentosIo[QNTMAXIO];  
    int qtdIos;           
    int ioAtual;
} Processo;

int processoAtual = 0;
Processo processos[QNTPROCESSOS];



GQueue *filaBloqueados;
GQueue *filaProntos;

void interControllerSim(int kernel){
    int segundos=0;
    while(1){
        sleep(1);
        segundos++;
        
        kill(kernel, SIGUSR1);// significa que acabou o timeslice 

        if(segundos % 3 == 0){ 
            kill(kernel, SIGUSR2); // significa que acabou o IO
        }
    }
}

void executaProcesso(int id, int pcInicial, int* momentosIo, int qtdIo, int pcFinal){
    int pc = pcInicial;
    int ioIndex = 0;

    while(pc < pcFinal && pc < QNTPCMAX) {
        printf("[A%d] - em execução, PC = %d\n", id, pc);
        

        if(ioIndex < qtdIo && pc == momentosIo[ioIndex]) {
            printf("[A%d] atingiu PC=%d e disparou Syscall de I/O!\n", id, pc);
            ioIndex++;
            
            kill(getppid(), SIGTTIN); 
            
            raise(SIGSTOP); 
        }

        pc++;
        sleep(1); 
    }

    if (pc >= QNTPCMAX) {
        printf("[A%d] ATINGIU O LIMITE MÁXIMO DO SISTEMA (%d) E FOI ENCERRADO.\n", id, QNTPCMAX);
    } else {
        printf("[A%d] CHEGOU AO SEU PC FINAL PRÓPRIO (%d) E TERMINOU NATURALMENTE.\n", id, pcFinal);
    }

    exit(0);
}

void criaProcesso(int id, Processo *processo,int pcFinal,int *momentosIo, int qtdIos) {
    processo->id = id;
    processo->pid = fork();
    processo->pc = 0;
    processo->pcFinal = pcFinal;
    processo->qtdIos = qtdIos;
    processo->ioAtual = 0;

    for(int i = 0; i < qtdIos; i++) {
        processo->momentosIo[i] = momentosIo[i];
    }
    
    if(processo->pid == 0){
        executaProcesso(id, processo->pc, processo->momentosIo, processo->qtdIos, processo->pcFinal);
        exit(0);
    }
    
    kill(processo->pid, SIGSTOP); 
    g_queue_push_tail(filaProntos, processo);
}


// tratadores de sinais 
void finalizaTimeSlice(int sinal){
    if (g_queue_is_empty(filaProntos)) return;

    Processo* processoAtual =(Processo*) g_queue_pop_head(filaProntos);
    processoAtual->pc++;

    if (waitpid(processoAtual->pid, NULL, WNOHANG) > 0) {
        printf("\n--> Kernel: [A%d] já havia finalizado.\n", processoAtual->id);
    } else {
        printf("\n--> Kernel: Fim do Timeslice de [A%d]. Salvando Contexto (PC = %d)\n", processoAtual->id, processoAtual->pc);
        kill(processoAtual->pid, SIGSTOP);
        g_queue_push_tail(filaProntos, processoAtual);
    }

    if (!g_queue_is_empty(filaProntos)) {
        Processo *proximoProcesso = (Processo *) g_queue_peek_head(filaProntos);
        printf("\n--> Kernel: Escalando [A%d]. Restaurando Contexto (PC = %d)\n", proximoProcesso->id, proximoProcesso->pc);
        kill(proximoProcesso->pid, SIGCONT);
    }

}

void bloqueiaProcesso(int sinal){
   if (g_queue_is_empty(filaProntos)) return;

    Processo *processoAtual = (Processo *) g_queue_pop_head(filaProntos);
    
    printf("\n--> Kernel: [A%d] realizou Syscall no PC = %d. Movido para a fila de Bloqueados.\n", processoAtual->id, processoAtual->pc);
    g_queue_push_tail(filaBloqueados, processoAtual);

    if (!g_queue_is_empty(filaProntos)) {
        Processo *proximoProcesso = (Processo *) g_queue_peek_head(filaProntos);
        printf("\n--> Kernel: Escalando [A%d] após bloqueio do anterior. (PC = %d)\n", proximoProcesso->id, proximoProcesso->pc);
        kill(proximoProcesso->pid, SIGCONT);
    }
}

void finalizaIO(int sinal){
    if (g_queue_is_empty(filaBloqueados)) return;

    Processo *processoLiberado = (Processo *) g_queue_pop_head(filaBloqueados);
    
    processoLiberado->pc++; 
    processoLiberado->ioAtual++;

    printf("\n--> Kernel: I/O Concluído para [A%d]. Retornando para a fila de Prontos (PC = %d).\n", processoLiberado->id, processoLiberado->pc);
    g_queue_push_tail(filaProntos, processoLiberado);

    if (g_queue_get_length(filaProntos) == 1) {
        kill(processoLiberado->pid, SIGCONT);
    }
}

// principal
int main(){
    filaBloqueados = g_queue_new();
    filaProntos = g_queue_new();
    
    pid_t kernel = fork();

    if(kernel == 0){
        
        int iosA1[] = {3, 7}; 
        int iosA2[] = {5};    
        int iosA3[] = {};   

        criaProcesso(1, &processos[0],4, iosA1, 2);
        criaProcesso(2, &processos[1],7, iosA2, 1);
        criaProcesso(3, &processos[2],3, iosA3, 0);
        
        if(!g_queue_is_empty(filaProntos)){
            Processo* primeiroProcesso = (Processo*) g_queue_peek_head(filaProntos);
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

    waitpid(kernel, NULL, 0);
    waitpid(interController, NULL, 0);
    
    return(1);
}


