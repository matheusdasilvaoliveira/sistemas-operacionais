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
    
    pid_t pidFilho = fork();
    
    if (pidFilho < 0) {
        printf("Erro ao criar processo filho.");
        return 1;
    }
    
    // Processo filho
    if (pidFilho == 0) {

        // isso influencia em tempo de que?
        // n = n + 3;
        pid_t pidNeto = fork();
        
        
        if (pidNeto < 0) {
            printf("Erro ao criar processo neto.");
            return 1;
        }
        
        // Processo neto
        if (pidNeto == 0) {
            for (int i = 0; i < 1000; i++) {
                n = n + 100;
            }
            printf("\nprocesso neto, pid=%d, n=%d", getpid(), n);
            exit(0);
        }
        else {
            // Espera o processo neto
            waitpid(pidNeto, NULL, 0);
            
            for (int i = 0; i < 1000; i++) {
                n = n + 10;
            }
            printf("\nprocesso filho, pid=%d, n=%d", getpid(), n);
        }
    }
    
    // Processo pai
    else {
        // Espera o processo filho
        waitpid(pidFilho, NULL, 0);
        for (int i = 0; i < 1000; i++) {
            n++;
        }
        printf("\nprocesso pai, pid=%d, n=%d", getpid(), n);
    }
    return 0;
}