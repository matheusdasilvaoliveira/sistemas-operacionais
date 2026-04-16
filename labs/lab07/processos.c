#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_PROCESSOS 10
#define TAMANHO_VETOR 10000

int main() {
    int shmid = shmget(IPC_PRIVATE, TAMANHO_VETOR * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    int *vetor = shmat(shmid, 0, 0);
    
    for(int i = 0; i<TAMANHO_VETOR; i++){
        vetor[i] = 5;
    }
    
    struct timeval inicio, fim;
    
    for(int p = 0; p < NUM_PROCESSOS; p++){
        if(fork() == 0){
            gettimeofday(&inicio, NULL);
            
            for(int i = 0;i<TAMANHO_VETOR;i++){
                vetor[i] =  vetor[i]*2 + 2;                
            }
            
            gettimeofday(&fim, NULL);
           
            double tempoExecTotal = (fim.tv_sec - inicio.tv_sec) + ((fim.tv_usec - inicio.tv_usec) / 1000000.0);
            printf("O tempo de execução do Processo [%d] foi: %f segundos\n", p, tempoExecTotal);

            exit(0);
        }
    }
 
    for(int i = 0; i < NUM_PROCESSOS; i++){
        wait(NULL);
    }
    
    int ehIgual = 1;
    
    for(int i = 1; i < TAMANHO_VETOR; i++){
        if(vetor[i]!=vetor[0]){
            ehIgual=0;
        }  
    }
    
    if(ehIgual){
        printf("\nTodos os espaços são iguais.\n");
    } else{
        printf("\nNem todos os espaços são iguais.\n");
    }

    shmdt(vetor);
    shmctl(shmid, IPC_RMID, 0);
    
    return 0;
}