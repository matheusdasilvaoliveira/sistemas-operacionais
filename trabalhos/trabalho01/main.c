#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// [[pid, pc da CPU], [pid, pc da CPU]]
int processos[3][2];
int qntProcessos = 3;

void kernelSim(){
   
}

void interControllerSim(){
       while(1){
        printf("[controller], em execução...\n");
        sleep(1);
    }
}

void criaProcesso(int id, int *processo) {
    int tempoCPU = processo[1];

    processo[0] = fork();
    processo[1] = 0;

    if(processo[0]==0){
        kill(processo[0], SIGSTOP); 
        exit(0);
    }

    printf("Processo [A%d] criado | pc = %d\n", id, processo[1]);
}

void processoAplicacao(int id, int pc){
    while(1){
        printf("[A%d], em execução, pc = %d\n", id, pc);
        pc++;
        sleep(1);
    }
}

int main(){
    // printf("Entre com a quantidade de processos entre 3 a 6: ");
    // scanf("%d", &qntProcessos);

    // while(qntProcessos < 3 || qntProcessos > 6){

    //     printf("Valor invalido, digite entre 3 e 6: ");
    //     scanf("%d", &qntProcessos);
        
    // }

    // pid_t pid[qntProcessos];

    // pid = fork();

    // if(pid == 0){
    //     // interControllerSim();
    //     exit(0);
    // }

    pid_t kernel = fork();

    if(kernel == 0){
        for(int i = 0; i<qntProcessos;i++){
            criaProcesso((i + 1), processos[i]);

        }
    
        int atual=0;

        while(1){
            printf("[kernel], em execução...\n");
            sleep(1);

            kill(pid[atual], SIGCONT);
            sleep(3);
            kill(pid[atual], SIGSTOP);
            atual = (atual+1)%qntProcessos;
    
            sleep(10);
        }

            exit(0);
        
    }

    
    
    return(1);
}


