#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#define TAMANHO_VETOR 100000
#define NUMERO_PROCESSOS 100

int main(int argc, char *argv[]) {
    int shmid = shmget(IPC_PRIVATE, TAMANHO_VETOR * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    int *vetor = shmat(shmid, 0, 0);
    
    for(int indice = 0; indice < TAMANHO_VETOR; indice++){
        vetor[indice] = 10;
    }

    for(int processo = 1; processo <= NUMERO_PROCESSOS; processo++){
        int pidProcesso = fork();

        if (pidProcesso < 0) {
            perror("Erro no fork");
            exit(1);
        }

        if(pidProcesso == 0){
            for(int indice = 0; indice < TAMANHO_VETOR; indice++){

                if(processo % 2 == 0){
                    vetor[indice] += processo;

                }else{
                    vetor[indice] -= processo;

                }
            }

            exit(0);
        }
    }


    for (int processos = 0; processos < NUMERO_PROCESSOS; processos++) {
        wait(NULL);
    }

    int valorEsperado = 10;

    for (int processos = 1; processos <= NUMERO_PROCESSOS; processos++) {

        if (processos % 2 == 0) {
            valorEsperado += processos;
        } else {
            valorEsperado -= processos;
        }
    }

    printf("Valor esperado: %d\n", valorEsperado);


    int erros = 0;

    for (int indice = 0; indice < TAMANHO_VETOR; indice++) {

        if (vetor[indice] != valorEsperado) {

            erros++;

            if (erros <= 10) {
                printf("Erro na posição %d -> valor = %d\n", indice, vetor[indice]);
            }
        }
    }

    if (erros == 0) {
        printf("Nenhum erro de concorrência encontrado.\n");

    } else {
        printf("Total de erros encontrados: %d\n", erros);
        
    }

    
    shmdt(vetor);
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}