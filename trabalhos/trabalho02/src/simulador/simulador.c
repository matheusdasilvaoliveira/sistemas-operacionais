#include "simulador.h"

unsigned tempoAtual = 0;
unsigned long pageFaults = 0;
unsigned long paginasEscritas = 0;

int simular(char *algoritmo, char *arquivo, int tamanhoDaPagina, int tamanhoDaMemoria,
            unsigned long *pageFaultsTotal, unsigned long *paginasEscritasTotal) {
    if (!validaTamanhoDaPagina(tamanhoDaPagina)) return 1;
    if (!validaTamanhoDaMemoria(tamanhoDaMemoria)) return 1;
    if (!validaAlgoritmos(algoritmo)) return 1;

    FILE *arquivoLog = abreArquivo(arquivo);
    if (arquivoLog == NULL) return 1;

    int capacidade = 1000000;
    unsigned int *paginas = malloc(capacidade * sizeof(unsigned int));
    char *operacoes = malloc(capacidade * sizeof(char));

    if (paginas == NULL || operacoes == NULL) {
        printf("Erro ao alocar o trace.\n");
        fclose(arquivoLog);
        return 1;
    }

    int totalAcessos = 0;
    unsigned int enderecoVirtual;
    char operacaoLida;

    while (fscanf(arquivoLog, "%x %c", &enderecoVirtual, &operacaoLida) == 2) {
        if (totalAcessos == capacidade) {
            capacidade *= 2;
            paginas = realloc(paginas, capacidade * sizeof(unsigned int));
            operacoes = realloc(operacoes, capacidade * sizeof(char));

            if (paginas == NULL || operacoes == NULL) {
                printf("Erro ao realocar o trace.\n");
                free(paginas);
                free(operacoes);
                fclose(arquivoLog);
                return 1;
            }
        }

        paginas[totalAcessos] = calculaPagina(enderecoVirtual, tamanhoDaPagina);
        operacoes[totalAcessos] = operacaoLida;
        totalAcessos++;
    }

    fclose(arquivoLog);

    Pagina *tabelaDePaginas = malloc(MAX_PAGINAS * sizeof(Pagina));

    if (tabelaDePaginas == NULL) {
        printf("Erro ao alocar tabela de paginas.\n");
        free(paginas);
        free(operacoes);
        return 1;
    }

    inicializaTabelaDePaginas(tabelaDePaginas);

    int quantidadeDeQuadros = calculaQuantidadeDeQuadros(tamanhoDaMemoria, tamanhoDaPagina);
    Quadro *memoriaFisica = criaMemoriaFisica(quantidadeDeQuadros);

    if (memoriaFisica == NULL) {
        free(tabelaDePaginas);
        free(paginas);
        free(operacoes);
        return 1;
    }

    tempoAtual = 0;
    pageFaults = 0;
    paginasEscritas = 0;

    for (int i = 0; i < totalAcessos; i++) {
        tempoAtual++;
        unsigned int pagina = paginas[i];
        char operacao = operacoes[i];
        int resetNRU = strcmp(algoritmo, "NRU") == 0 && (tempoAtual % ACESSOS_RESET_NRU) == 0;

        if (resetNRU) {
            resetaBitsReferenciaNRU(memoriaFisica, tabelaDePaginas, quantidadeDeQuadros);
        }

        if (tabelaDePaginas[pagina].presente) {
            atualizaPagina(memoriaFisica, tabelaDePaginas, pagina, operacao);
            int quadro = tabelaDePaginas[pagina].quadro;
            memoriaFisica[quadro].tempo = tempoAtual;
        } else {
            pageFaults++;

            int quadroLivre = encontraQuadroVazio(memoriaFisica, quantidadeDeQuadros);

            if (quadroLivre != -1) {
                carregaPagina(memoriaFisica, tabelaDePaginas, quadroLivre, pagina, operacao);
                memoriaFisica[quadroLivre].tempo = tempoAtual;
            } else {
                int quadroSubstituido;

                if (strcmp(algoritmo, "LRU") == 0) {
                    quadroSubstituido = lru(memoriaFisica, quantidadeDeQuadros);
                } else if (strcmp(algoritmo, "CLOCK") == 0) {
                    quadroSubstituido = relogio(memoriaFisica, quantidadeDeQuadros);
                } else if (strcmp(algoritmo, "OTIMO") == 0) {
                    quadroSubstituido = otimo(memoriaFisica, quantidadeDeQuadros, paginas, totalAcessos, i);
                } else if (strcmp(algoritmo, "NRU") == 0) {
                    quadroSubstituido = nru(memoriaFisica, quantidadeDeQuadros);
                } else {
                    printf("Algoritmo não reconhecido ou erro de execução.\n");
                    free(tabelaDePaginas);
                    free(memoriaFisica);
                    free(paginas);
                    free(operacoes);
                    return 1;
                }

                if (memoriaFisica[quadroSubstituido].modificada) {
                    paginasEscritas++;
                }

                substituiPagina(memoriaFisica, tabelaDePaginas, quadroSubstituido, pagina, operacao);
                memoriaFisica[quadroSubstituido].tempo = tempoAtual;
            }
        }
    }

    if (pageFaultsTotal != NULL) {
        *pageFaultsTotal = pageFaults;
    }

    if (paginasEscritasTotal != NULL) {
        *paginasEscritasTotal = paginasEscritas;
    }

    free(tabelaDePaginas);
    free(memoriaFisica);
    free(paginas);
    free(operacoes);

    return 0;
}
