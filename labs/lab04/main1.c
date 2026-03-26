#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>

int main()
{
    // aloca a memória compartilhada
    int segmento = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    
    // associa a memória compartilhada ao processo
    int *n_compartilhado = (int*) shmat(segmento, 0, 0);
    
    *n_compartilhado = 1;
    
    pid_t pidFilho1 = fork();
    
    if (pidFilho1 < 0) {
        printf("Erro ao criar processo filho.");
        return 1;
    }
    
    // Processo filho1
    if (pidFilho1 == 0) {
        // waitpid(pidFilho1, NULL, 0);
        for (int i = 0; i < 1000; i++) {
            *n_compartilhado = * n_compartilhado + 1;
        }
        printf("processo filho1, pid=%d, n=%d\n", getpid(), *n_compartilhado);
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
            *n_compartilhado = *n_compartilhado + 10;
        }
        printf("processo filho2, pid=%d, n=%d\n", getpid(), *n_compartilhado);
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
            *n_compartilhado = *n_compartilhado + 100;
        }
        printf("processo filho3, pid=%d, n=%d\n", getpid(), *n_compartilhado);
        exit(0);
    }
    
    // Processo pai
   
    // Espera o processo filho
    waitpid(pidFilho1, NULL, 0);
    printf("processo pai, pid=%d, n=%d\n", getpid(), *n_compartilhado);
    
    // libera a memória compartilhada do processo
    shmdt (n_compartilhado);
    
    // libera a memória compartilhada
    shmctl (segmento, IPC_RMID, 0);

    return 0;
}