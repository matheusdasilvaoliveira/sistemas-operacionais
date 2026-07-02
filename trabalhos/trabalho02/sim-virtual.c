#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGINAS 1048576
#define ACESSOS_RESET_NRU 2000

unsigned tempoAtual = 0;
unsigned long pageFaults = 0;
unsigned long paginasEscritas = 0;

typedef struct {
    int presente;
    int quadro;
    int referenciada;
    int modificada;
} Pagina;

typedef struct {
    int pagina;
    int referenciada;
    int modificada;
    unsigned tempo;
} Quadro;

int validaTamanhoDaPagina(int tamanhoDaPagina){
    if (tamanhoDaPagina != 4 && tamanhoDaPagina != 8) {
        printf("Erro: tamanho da pagina deve ser 4 ou 8 KB.\n");
        return 0;
    }

    return 1;
}

int validaTamanhoDaMemoria(int tamanhoDaMemoria){
     if (tamanhoDaMemoria != 1 && tamanhoDaMemoria != 2 && tamanhoDaMemoria != 4) {
        printf("Erro: memoria deve ser 1, 2 ou 4 MB.\n");
        return 0;
    }

    return 1;
}

int validaAlgoritmos(char *algoritmo){
    if (strcmp(algoritmo, "LRU") != 0 && strcmp(algoritmo, "NRU") != 0 && strcmp(algoritmo, "CLOCK") != 0 && strcmp(algoritmo, "OTIMO") != 0) {
            printf("Algoritmo invalido.\n");
            return 0;
    }

    return 1;
}

void inicializaTabelaDePaginas(Pagina tabelaDePaginas[]) {
    for (int i = 0; i < MAX_PAGINAS; i++) {
        tabelaDePaginas[i].presente = 0;
        tabelaDePaginas[i].quadro = -1;
        tabelaDePaginas[i].referenciada = 0;
        tabelaDePaginas[i].modificada = 0;
    }
}

void inicializaMemoriaFisica(Quadro memoriaFisica[], int quantidadeDeQuadros) {

    for (int i = 0; i < quantidadeDeQuadros; i++) {
        memoriaFisica[i].pagina = -1;
        memoriaFisica[i].referenciada = 0;
        memoriaFisica[i].modificada = 0;
        memoriaFisica[i].tempo = 0;
    }
}

unsigned int calculaPagina(unsigned int enderecoVirtual, int tamanhoDaPagina) {
    if (tamanhoDaPagina == 4)
        return enderecoVirtual >> 12;

    return enderecoVirtual >> 13;
}

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

int calculaQuantidadeDeQuadros(int tamanhoDaMemoria, int tamanhoDaPagina) {
    return (tamanhoDaMemoria * 1024) / tamanhoDaPagina;
}

void resetaBitsReferenciaNRU(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quantidadeDeQuadros) {
    for (int numQuadro = 0; numQuadro < quantidadeDeQuadros; numQuadro++) {
        memoriaFisica[numQuadro].referenciada = 0;
        if (memoriaFisica[numQuadro].pagina != -1) {
            tabelaDePaginas[memoriaFisica[numQuadro].pagina].referenciada = 0;
        }
    }
}

int encontraQuadroVazio(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    for (int i = 0; i < quantidadeDeQuadros; i++) {
        if (memoriaFisica[i].pagina == -1) {
            return i;
        }
    }

    return -1;
}

Quadro *criaMemoriaFisica(int quantidadeDeQuadros) {

    Quadro *memoriaFisica = malloc(quantidadeDeQuadros * sizeof(Quadro));

    if (memoriaFisica == NULL) {
        printf("Erro ao alocar memoria fisica.\n");
        return NULL;
    }

    inicializaMemoriaFisica(memoriaFisica, quantidadeDeQuadros);

    return memoriaFisica;
}

void carregaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int pagina, char operacao) {
    memoriaFisica[quadro].pagina = pagina;
    memoriaFisica[quadro].referenciada = 1;
    memoriaFisica[quadro].modificada = (operacao == 'W');

    tabelaDePaginas[pagina].presente = 1;
    tabelaDePaginas[pagina].quadro = quadro;
    tabelaDePaginas[pagina].referenciada = 1;
    tabelaDePaginas[pagina].modificada = (operacao == 'W');
}

void atualizaPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], unsigned int pagina, char operacao) {
    int quadro = tabelaDePaginas[pagina].quadro;

    tabelaDePaginas[pagina].referenciada = 1;
    memoriaFisica[quadro].referenciada = 1;

    if (operacao == 'W') {
        tabelaDePaginas[pagina].modificada = 1;
        memoriaFisica[quadro].modificada = 1;
    }
}

void substituiPagina(Quadro memoriaFisica[], Pagina tabelaDePaginas[], int quadro, unsigned int novaPagina, char operacao) {

    unsigned int paginaAntiga = memoriaFisica[quadro].pagina;

    tabelaDePaginas[paginaAntiga].presente = 0;
    tabelaDePaginas[paginaAntiga].quadro = -1;
    tabelaDePaginas[paginaAntiga].referenciada = 0;
    tabelaDePaginas[paginaAntiga].modificada = 0;

    carregaPagina(memoriaFisica, tabelaDePaginas, quadro, novaPagina, operacao);
}

int lru(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    int quadroLRU = 0;

    for (int i = 1; i < quantidadeDeQuadros; i++) {
        if (memoriaFisica[i].tempo < memoriaFisica[quadroLRU].tempo) {
            quadroLRU = i;

        }

    }
    
    return quadroLRU;
}

int relogio(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    static int ponteiro = 0;

    while (memoriaFisica[ponteiro].referenciada == 1) {
        memoriaFisica[ponteiro].referenciada = 0;
        ponteiro = (ponteiro + 1) % quantidadeDeQuadros;
    }

    int vitima = ponteiro;
    ponteiro = (ponteiro + 1) % quantidadeDeQuadros;

    return vitima;
}

int otimo(Quadro memoriaFisica[], int quantidadeDeQuadros, unsigned int paginas[], int totalAcessos, int indiceAtual) {
    int vitima = 0;
    int maisLonge = -1;

    for (int q = 0; q < quantidadeDeQuadros; q++) {
        int proximoUso = totalAcessos;

        for (int j = indiceAtual + 1; j < totalAcessos; j++) {
            if (paginas[j] == (unsigned int) memoriaFisica[q].pagina) {
                proximoUso = j;
                break;
            }
        }

        if (proximoUso > maisLonge) {
            maisLonge = proximoUso;
            vitima = q;
        }
    }

    return vitima;
}

int nru(Quadro memoriaFisica[], int quantidadeDeQuadros) {
    int quadroVitima = -1;
    int menorClasse = 4;

    for (int numQuadro = 0; numQuadro < quantidadeDeQuadros; numQuadro++) {
        int flagReferenciada = memoriaFisica[numQuadro].referenciada;
        int flagModificada = memoriaFisica[numQuadro].modificada;
        int classeAtual;

        if (flagReferenciada == 0 && flagModificada == 0) classeAtual = 0;
        else if (flagReferenciada == 0 && flagModificada == 1) classeAtual = 1;
        else if (flagReferenciada == 1 && flagModificada == 0) classeAtual = 2;
        else if (flagReferenciada == 1 && flagModificada == 1) classeAtual = 3;

        if (classeAtual < menorClasse) {
            menorClasse = classeAtual;
            quadroVitima = numQuadro;

            if (menorClasse == 0) {
                break;
            }
        }
    }
    return quadroVitima;
}

int main(int argc, char *argv[]) {

    if (argc != 5) {
        printf("Uso: ./sim-virtual <algoritmo> <arquivo.log> <tamPaginaKB> <tamMemoriaMB>\n");
        return 1;
    }

    char *algoritmo = argv[1];
    char *arquivo = argv[2];
    int tamanhoDaPagina = atoi(argv[3]);
    int tamanhoDaMemoria = atoi(argv[4]);

    //validações dos parametros
    if (!validaTamanhoDaPagina(tamanhoDaPagina)) return 1;
    if (!validaTamanhoDaMemoria(tamanhoDaMemoria)) return 1;
    if (!validaAlgoritmos(algoritmo)) return 1;

    //abrindo o arquivo de log
    FILE *arquivoLog = abreArquivo(arquivo);
    if (arquivoLog == NULL) return 1;

    //pre-carregando todo o trace (necessario para o algoritmo otimo enxergar o futuro)
    int capacidade = 1000000;
    unsigned int *paginas = malloc(capacidade * sizeof(unsigned int));
    char *operacoes = malloc(capacidade * sizeof(char));

    if (paginas == NULL || operacoes == NULL) {
        printf("Erro ao alocar o trace.\n");
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
                return 1;
            }
        }

        paginas[totalAcessos] = calculaPagina(enderecoVirtual, tamanhoDaPagina);
        operacoes[totalAcessos] = operacaoLida;
        totalAcessos++;
    }

    fclose(arquivoLog);

    //criando a tabela de paginas
    Pagina *tabelaDePaginas = malloc(MAX_PAGINAS * sizeof(Pagina));

    if (tabelaDePaginas == NULL) {
        printf("Erro ao alocar tabela de paginas.\n");
        return 1;
    }

    inicializaTabelaDePaginas(tabelaDePaginas);

    //criando os quadros
    int quantidadeDeQuadros = calculaQuantidadeDeQuadros(tamanhoDaMemoria, tamanhoDaPagina);
    Quadro *memoriaFisica = criaMemoriaFisica(quantidadeDeQuadros);
    
    if (memoriaFisica == NULL) return 1;

    // toda a logica na memoria de paginação
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

    printf("Executando o simulador...\n");
    printf("Arquivo de entrada: %s\n", arquivo);
    printf("Tamanho da memoria fisica: %d MB\n", tamanhoDaMemoria);
    printf("Tamanho das paginas: %d KB\n", tamanhoDaPagina);
    printf("Algoritmo de substituicao: %s\n", algoritmo);
    printf("Numero de Faltas de Paginas: %lu\n", pageFaults);
    printf("Numero de Paginas Escritas: %lu\n", paginasEscritas);

    free(tabelaDePaginas);
    free(memoriaFisica);
    free(paginas);
    free(operacoes);
    return 0;
}
