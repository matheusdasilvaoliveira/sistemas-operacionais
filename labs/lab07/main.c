#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define TAMANHO 100
int main()
{
    int vetor[TAMANHO];
    
    for(int i = 0; i<TAMANHO; i++){
        vetor[i] = 5;
    }
    
    int p;
    
    for(p = 0; p < 10; p++){
        
        if( fork() == 0){
                struct timeval T;
                gettimeofday(&T, NULL);
            for(int i = 0;i<TAMANHO;i++){
                
                long tempoExecMin = T.tv_sec / 60;
                
                vetor[i] =  vetor[i]*2 + 2;
                
         

                printf("No processo: %d, o valor do espaço é: %d\n\n", p, vetor[i]);
                printf("O tempo de execução foi: %ld\n", tempoExecMin);
            }
    
            exit(0);
        }
        
        
    }
    
    for(int i = 0; i<10; i++){
        wait(NULL);
    }
    
    int ehIgual = 1;
    
    for(int i = 1; i<TAMANHO; i++){
        if(vetor[i]!=vetor[0]){
            ehIgual=0;
        }
            
    }
    
    
    if(ehIgual){
        printf("Todos os espaços são iguais");
    }else{
        printf("nem todos os espaços são iguais");
    }
    
    return 0;
}