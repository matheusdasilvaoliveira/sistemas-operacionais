#include "arquivo.h"

FILE *abreArquivo(char *arquivo) {
    char caminho[100];
    sprintf(caminho, "logs/%s", arquivo);

    FILE *arquivoLog = fopen(caminho, "r");
    if (arquivoLog == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", caminho);
        return NULL;
    }

    return arquivoLog;
}
