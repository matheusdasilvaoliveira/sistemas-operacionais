#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>


// Função que cada filho vai executar
void rotina_filho(char *nome) {
    while(1) {
        printf("[%s] executando na CPU...\n", nome);
        sleep(1); // Pausa para não inundar o terminal
    }
}

int main() {
    __pid_t p1, p2, p3;

    // 1. Os 3 processos filhos
    if ((p1 = fork()) == 0) {
        rotina_filho("P1");
        exit(0);
    }
    if ((p2 = fork()) == 0) {
        rotina_filho("P2");
        exit(0);
    }
    if ((p3 = fork()) == 0) {
        rotina_filho("P3");
        exit(0);
    }

    // 2. O pai pausa todos eles imediatamente após a criação
    // Assim, nenhum executa até que o escalonador autorize
    kill(p1, SIGSTOP);
    kill(p2, SIGSTOP);
    kill(p3, SIGSTOP);

    int processo_ativo = 0; // Guarda quem está na CPU no momento (1, 2 ou 3)

    printf("Escalonador Real-Time iniciado! Monitorando o tempo...\n");

    // 3. Loop infinito do Escalonador
    int i = 0;
    while(i < 1000) {
        struct timeval T;
        gettimeofday(&T, NULL);
        
        // Relógio de 1 minuto
        int seg = T.tv_sec % 60;

        int deve_executar = 0;

        // 4. Lógica de escalonamento baseada no tempo
        if (seg >= 5 && seg <= 24) {
            deve_executar = 1; // P1 executa por 20 seg (do seg 5 ao 24)
        } else if (seg >= 45 && seg <= 59) {
            deve_executar = 2; // P2 executa por 15 seg (do seg 45 ao 59)
        } else {
            deve_executar = 3; // P3 executa no restante do tempo
        }

        // 5. Troca de Contexto (Context Switch)
        // Só envia os sinais se a fatia de tempo virou e um novo processo deve assumir
        if (deve_executar != processo_ativo) {
            // Pausa o processo que estava na CPU
            if (processo_ativo == 1) {
                kill(p1, SIGSTOP);
            }
            if (processo_ativo == 2) {
                kill(p2, SIGSTOP);
            }
            if (processo_ativo == 3) {
                kill(p3, SIGSTOP);
            }

            // Acorda o novo processo para usar a CPU
            if (deve_executar == 1) {
                kill(p1, SIGCONT);
            }
            if (deve_executar == 2) {
                kill(p2, SIGCONT);
            }
            if (deve_executar == 3) {
                kill(p3, SIGCONT);
            }

            processo_ativo = deve_executar;
            printf("\n--- TEMPO: %02ds | MUDANÇA DE CONTEXTO: Processo P%d assumiu a CPU ---\n\n", seg, processo_ativo);
        }

        // Pequeno delay de 100ms no loop do escalonador para evitar "Busy Waiting" agressivo, 
        // poupando a CPU, mas mantendo a precisão na verificação do tempo.
        usleep(100000);
        i++;
    }

    return 0;
}