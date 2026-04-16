#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define TAMANHO_VETOR 10000
#define NUM_THREADS 10

int vetor[TAMANHO_VETOR];

void *alteraVetor(void *args) {
    int threadId = (*(int*) args);
    struct timeval inicio, fim;

    gettimeofday(&inicio, NULL);

    for (int i=0; i < TAMANHO_VETOR; i++) {
        vetor[i] = vetor[i]*2 + 2;
    }
    
    gettimeofday(&fim, NULL);
    
    double tempoExecTotal = (fim.tv_sec - inicio.tv_sec) + ((fim.tv_usec - inicio.tv_usec) / 1000000.0);
    printf("O tempo de execução da Thread [%d] foi: %f segundos\n", threadId, tempoExecTotal);

    pthread_exit(NULL);
}

int main() {
    for(int i = 0; i<TAMANHO_VETOR; i++){
        vetor[i] = 5;
    }

    pthread_t threads[NUM_THREADS];
    pthread_t threadIds[NUM_THREADS];

    for(int i=0; i < NUM_THREADS; i++) {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, alteraVetor, (void *)&threadIds[i]);
        
    }

    for(int i=0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    int ehIgual = 1;
    
    for(int i = 1; i<TAMANHO_VETOR; i++){
        if(vetor[i]!=vetor[0]){
            ehIgual=0;
            break;
        }
    }
    
    if(ehIgual){
        printf("\nTodos os espaços são iguais.\n");
    }else{
        printf("\nNem todos os espaços são iguais.\n");
    }
    
    return 0;
}