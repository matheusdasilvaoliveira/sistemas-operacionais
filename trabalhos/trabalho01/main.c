#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int pid[3];

void kernelSim(){
       while(1){
        printf("[kernel], em execução...\n");
        sleep(1);
    }
}

void interControllerSim(){
       while(1){
        printf("[controller], em execução...\n");
        sleep(1);
    }
}

void processoAplicacao(int id){
    int pc=0;

    while(1){
        printf("[A%d], em execução, pc = %d\n", id, pc);
        pc++;
        sleep(1);
    }
}

int main(){
    int qntProcessos;

    printf("Entre com a quantidade de processos entre 3 a 6: ");
    scanf("%d", &qntProcessos);

    while(qntProcessos < 3 || qntProcessos > 6){

        printf("Valor invalido, digite entre 3 e 6: ");
        scanf("%d", &qntProcessos);
        
    }

    // pid_t pid[qntProcessos];

    // pid = fork();

    // if(pid == 0){
    //     // interControllerSim();
    //     exit(0);
    // }

    pid_t processo = fork();

    if(processo == 0){
        kernelSim();
        exit(0);
    }

    for(int i = 0; i<qntProcessos;i++){
        pid[i] = fork();

        if(pid[i]==0){
            processoAplicacao(i+1);
            exit(0);
        }

        kill(pid[i],SIGSTOP); 
    }
    int atual=0;

    while(1){
        kill(pid[atual], SIGCONT);
        sleep(3);
        kill(pid[atual], SIGSTOP);
        atual = (atual+1)%qntProcessos;
   
        sleep(10);
    }
    return(1);
}


