#include "common.h"
#include "pokedex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#define BUFSZ 1024
#define MAX_CONN_QUEUE 10
#define TAM_MAX_POKEDEX 40


void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    //Reutiliza uma porta caso ela já tenha sido utilizada
    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    //O 10 é a quantidade de conexões que podem estar pendentes para tratamento
    if (0 != listen(s, MAX_CONN_QUEUE)) {
        logexit("listen");
    }

    //Mensagem para indicar que estamos esperando
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    struct Pokemon pokedex[TAM_MAX_POKEDEX];
    //Fica tratando eternamente os clientes
    while (1) {
        //storage do cliente
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        //Retorna um novo socket
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        //Printa mensagem de conexão
        char caddrstr[BUFSZ];
        /*addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);*/

        //Recebe uma mensagem do cliente com o recv
        //Nesse caso, estamos assumindo que o cliente não vai enviar mensagens por partes
        //mas sim, tudo de uma vez. Se não, deveríamos fazer um while igual no cliente
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0);
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        //Envia uma resposta
        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf) + 1, 0);
        //Se não enviar o número certo de dados
        if (count != strlen(buf) + 1) {
            logexit("send");
        }
        //Fecha conexão
        close(csock);
    }

    exit(EXIT_SUCCESS);
}