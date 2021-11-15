#include "common.h"
#include "pokedex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#include <sys/types.h>

#define MAX_CONN_QUEUE 10

struct Server{
    struct sockaddr *addr;
    struct sockaddr_storage storage;
    int my_socket;
    char addrstr[BUFSZ];
};

struct Client{
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr;
    socklen_t caddrlen;
    char caddrstr[BUFSZ];
    int socket;
};

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void iniciar_servidor(struct Server *my_server){
    my_server->my_socket = socket(my_server->storage.ss_family, SOCK_STREAM, 0);
    if (my_server->my_socket == -1) {
        logexit("socket");
    }

    //Reutiliza uma porta caso ela já tenha sido utilizada
    int enable = 1;
    if (0 != setsockopt(my_server->my_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    //Liga o server a uma porta
    my_server->addr = (struct sockaddr *)(&my_server->storage);
    if (0 != bind(my_server->my_socket, my_server->addr, sizeof(my_server->storage))) {
        logexit("bind");
    }

    //O MAX_CONN_QUEUE é a quantidade de conexões que podem estar pendentes para tratamento
    if (0 != listen(my_server->my_socket, MAX_CONN_QUEUE)) {
        logexit("listen");
    }

    //Mensagem para indicar que estamos esperando
    addrtostr(my_server->addr, my_server->addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", my_server->addrstr);

    return;
}

void iniciar_client(struct Client *my_client, struct Server *my_server){
    my_client->caddr = (struct sockaddr *)(&(my_client->cstorage));
    my_client->caddrlen = sizeof(my_client->cstorage);

    //Retorna um novo socket
    my_client->socket = accept(my_server->my_socket, my_client->caddr, &(my_client->caddrlen));

    if (my_client->socket == -1) {
        logexit("accept");
    }

    //Printa mensagem de conexão
    addrtostr(my_client->caddr, my_client->caddrstr, BUFSZ);
    printf("[log] connection from %s\n", my_client->caddrstr);
}

bool conversa_client_server(struct Client *my_client){
    bool matar_server = false;
    while(1){
        //Recebe uma mensagem do cliente com o recv
        char buf[TAM_MAX_MSG];
        memset(buf, 0, TAM_MAX_MSG);

        size_t bytes_recebidos_pacote;
        unsigned int total_bytes_recebido = 0;
        bool cliente_desconectou = false;

        printf("[log] Esperando mensagem chegar!\n");
        while(1){
            printf("Esperando pacote!\n");
            bytes_recebidos_pacote = recv(my_client->socket, buf + total_bytes_recebido, TAM_MAX_MSG - 1, 0);
            if(bytes_recebidos_pacote <= 0){
                //printf("[log] Chegou 0 ou negativo pacotes!\n");
                cliente_desconectou = true;
                break;
            }
            bool fim_msg_detectado = false;
            for(int i=total_bytes_recebido;i<(TAM_MAX_MSG-total_bytes_recebido);i++){
                //printf("Testando no Server %d\n",i);
                char barra_n[2] = "\n";
                char fim_string[2] = "\0";
                /*if(strcmp(&buf[i],fim_string)==0){
                    printf("Possui \\0 em %d\n",i);
                }*/
                if(strcmp(&buf[i],barra_n)==0){
                    fim_msg_detectado = true;
                    //printf("Possui \\n em %d\n",i);
                    break;
                }
            }
            total_bytes_recebido += bytes_recebidos_pacote;
            if(fim_msg_detectado){
                break;
            }
        }

        if(cliente_desconectou){
            printf("Client se desconectou repentinamente!\n");
            //close(csock);
            break;
        }
                
        //CONFERIR O CONTEUDO DA MENSAGEM. DAR CLOSE SE NECESSÁRIO
        if(strcmp(buf,"kill\n")==0){
            printf("Pediu para matar o servidor\n");
            matar_server = true;
            //Fecha conexão
            close(my_client->socket);
            break;
        }

        if(strcmp(buf, "logout\n")==0){
            cliente_desconectou=true;
            printf("Client fez logout!");
            close(my_client->socket);
            break;
        }
                
        //Usar fputs ao invés do printf
        printf("< ");
        fputs(buf, stdout);
        printf("[msg] %s, %d bytes: \'%s\'", my_client->caddrstr, (int)bytes_recebidos_pacote, buf);

        //Envia uma resposta
        //sprintf(buf, "remote endpoint: %.500s\n", caddrstr);

        bytes_recebidos_pacote = send(my_client->socket, buf, strlen(buf) + 1, 0);

        //Se não enviar o número certo de dados
        if (bytes_recebidos_pacote != strlen(buf) + 1) {
            logexit("send");
        }
    }

    return matar_server;
}

int main(int argc, char **argv) {
    
    if (argc < 3) {
        usage(argc, argv);
    }

    struct Server my_server;

    if (0 != server_sockaddr_init(argv[1], argv[2], &(my_server.storage))) {
        usage(argc, argv);
    }
    
    iniciar_servidor(&my_server);

    struct Pokedex minhaPokedex;
    bool matar_server = false;

    //Fica tratando eternamente os clientes
    while (!matar_server) {
        printf("Esperando conexao\n");
        //storage do cliente
        struct Client my_client;
        iniciar_client(&my_client, &my_server);
        matar_server = conversa_client_server(&my_client);
    }
    exit(EXIT_SUCCESS);
}