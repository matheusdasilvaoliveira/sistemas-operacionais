#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINHA 1024
#define MAX_ARGS 64

int main(int argc, char *argv[]) {
    char linha[MAX_LINHA];
    char *args[MAX_ARGS];
    
    while (1) {
        printf("'3' ");
        if (!fgets(linha, MAX_LINHA, stdin)) break;
        linha[strcspn(linha, "\n")] = '\0';

        if (strcmp(linha, ":q") == 0) break;

        int i = 0;
        char *token = strtok(linha, " ");
        
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid_t pid = fork();

        if (pid < 0) {
            printf("Erro: ao criar processo filho.\n");
            return 1;
        }
        
        if (pid == 0) {
            if (args[0] == NULL) break;
            
            execvp(linha, args);

            perror("Comando não encontrado.");
            exit(1);
        }
        else {
            wait(NULL);
        }
    }
    return 0;
}
