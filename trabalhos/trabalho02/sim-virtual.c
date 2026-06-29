#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGINAS 1048576
unsigned tempoAtual = 0;

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
    if (strcmp(algoritmo, "LRU") != 0 && strcmp(algoritmo, "NRU") != 0 && strcmp(algoritmo, "CLOCK") != 0 &&strcmp(algoritmo, "OTIMO") != 0) {
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
    printf("Executando LRU...\n");
    int quadroLRU = 0;

    for (int i = 1; i < quantidadeDeQuadros; i++) {
        if (memoriaFisica[i].tempo < memoriaFisica[quadroLRU].tempo) {
            quadroLRU = i;

        }

    }
    
    return quadroLRU;
}

int clock();
int nru();
int otimo();

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
    unsigned int enderecoVirtual;
    char operacao;

    while (fscanf(arquivoLog, "%x %c", &enderecoVirtual, &operacao) == 2) {
        tempoAtual++;
        unsigned int pagina = calculaPagina(enderecoVirtual, tamanhoDaPagina);

        if (tabelaDePaginas[pagina].presente) {

            atualizaPagina(memoriaFisica, tabelaDePaginas, pagina, operacao);
            int quadro = tabelaDePaginas[pagina].quadro;
            memoriaFisica[quadro].tempo = tempoAtual;

        } else {
            printf("\n\nPage Fault na pagina %u.\n", pagina);

            int quadroLivre = encontraQuadroVazio(memoriaFisica, quantidadeDeQuadros);

            printf("Quadro livre: %d\n", quadroLivre);
            if (quadroLivre != -1) {
                carregaPagina(memoriaFisica, tabelaDePaginas, quadroLivre, pagina, operacao);
                memoriaFisica[quadroLivre].tempo = tempoAtual;
                printf("Pagina %u carregada no quadro %d.\n", pagina, quadroLivre);

            } else {
            printf("Memoria cheia! ");
               int quadroSubstituido;

                if (strcmp(algoritmo, "LRU") == 0) {
                    quadroSubstituido = lru(memoriaFisica, quantidadeDeQuadros);}

                // } else if (strcmp(algoritmo, "CLOCK") == 0) {
                //     quadroSubstituido = clock();

                // } else if (strcmp(algoritmo, "NRU") == 0) {
                //     quadroSubstituido = nru();

                // } else {
                //     quadroSubstituido = otimo();

                // }

            printf("\n\nQuadro escolhido: %d\n", quadroSubstituido);
            printf("Pagina antiga: %d\n", memoriaFisica[quadroSubstituido].pagina);
            printf("Pagina nova: %u\n", pagina);

            substituiPagina(memoriaFisica, tabelaDePaginas, quadroSubstituido, pagina, operacao);
            memoriaFisica[quadroSubstituido].tempo = tempoAtual;

            printf("Substituicao realizada!\n\n");

            }
        }
    }

    free(tabelaDePaginas);
    free(memoriaFisica);
    fclose(arquivoLog);
    return 0;
}