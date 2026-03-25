#include <stdio.h>

int main(int argc, char *argv[]) {
    FILE *arquivo;
    int caractere;

    if (argc < 2) {
        printf("Erro: Nome do arquivo não fornecido.\n");
        return 1;
    }
    
    arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        printf("Erro: Arquivo vazio ou não encontrado.");
        return 1;
    }

    while ((caractere = fgetc(arquivo)) != EOF) {
        putchar(caractere);
    }

    fclose(arquivo);

    return 0;
}