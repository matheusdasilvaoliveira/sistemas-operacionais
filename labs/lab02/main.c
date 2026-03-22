/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main()
{
    int n = 1;
    
    pid_t pidFilho1 = fork();
    
    if (pidFilho1 < 0) {
        printf("Erro ao criar processo filho.");
        return 1;
    }
    
    // Processo filho1
    if (pidFilho1 == 0) {
        // waitpid(pidFilho1, NULL, 0);
        for (int i = 0; i < 1000; i++) {
            n++;
        }
        printf("\nprocesso filho1, pid=%d, n=%d", getpid(), n);
        exit(0);
    }
    
    pid_t pidFilho2 = fork();

    if (pidFilho2 < 0) {
        printf("Erro ao criar processo filho.");
        return 1;
    }
    
    // Processo filho2
    if (pidFilho2 == 0) {
        // waitpid(pidFilho2, NULL, 0);
        for (int i = 0; i < 1000; i++) {
            n = n + 10;
        }
        printf("\nprocesso filho2, pid=%d, n=%d", getpid(), n);
        exit(0);
    }
    
    pid_t pidFilho3 = fork();

    if (pidFilho3 < 0) {
        printf("Erro ao criar processo filho.");
        return 1;
    }
    
    // Processo filho3
    if (pidFilho3 == 0) {
        // waitpid(pidFilho3, NULL, 0);
        for (int i = 0; i < 1000; i++) {
            n = n + 100;
        }
        printf("\nprocesso filho3, pid=%d, n=%d", getpid(), n);
        exit(0);
    }
    
    // Processo pai
   
    // Espera o processo filho
    waitpid(pidFilho1, NULL, 0);
    printf("\nprocesso pai, pid=%d, n=%d", getpid(), n);

    return 0;
}