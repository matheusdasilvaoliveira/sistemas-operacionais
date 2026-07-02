CC = gcc
CFLAGS = -Wall
TARGET = sim-virtual
SRCS = \
	src/main.c \
	src/simulador/simulador.c \
	src/memoria/memoria.c \
	src/entrada_saida/arquivo.c \
	src/entrada_saida/saida.c \
	src/algoritmos/lru.c \
	src/algoritmos/relogio.c \
	src/algoritmos/otimo.c \
	src/algoritmos/nru.c

compila:
	$(CC) $(CFLAGS) -I src/simulador -I src/memoria -I src/entrada_saida -I src/algoritmos \
    $(SRCS) \
    -o $(TARGET)
