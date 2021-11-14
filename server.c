#include "common.h"
#include "pokedex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#define MAX_CONN_QUEUE 10


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

    struct Pokedex minhaPokedex;
    short int matar_server = 0;
    //Fica tratando eternamente os clientes
    while (1) {
        if(matar_server == 0){
            printf("Esperando conexao\n");
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
            addrtostr(caddr, caddrstr, BUFSZ);
            printf("[log] connection from %s\n", caddrstr);
            

            while(1){
                //Recebe uma mensagem do cliente com o recv
                char buf[TAM_MAX_MSG];
                memset(buf, 0, TAM_MAX_MSG);
                size_t count;
                unsigned int total = 0;
                short int desconectou = 0;
                printf("[log] Esperando mensagem chegar!\n");
                while(1){
                    printf("Esperando pacote!\n");
                    count = recv(csock, buf + total, TAM_MAX_MSG - 1, 0);
                    if(count <= 0){
                        //printf("[log] Chegou 0 ou negativo pacotes!\n");
                        desconectou = 1;
                        break;
                    }
                    short int jaLeu = 0;
                    for(int i=total;i<(TAM_MAX_MSG-total);i++){
                        //printf("Testando no Server %d\n",i);
                        char barra_n[2] = "\n";
                        char fim_string[2] = "\0";
                        /*if(strcmp(&buf[i],fim_string)==0){
                            printf("Possui \\0 em %d\n",i);
                        }*/
                        if(strcmp(&buf[i],barra_n)==0){
                            jaLeu = 1;
                            //printf("Possui \\n em %d\n",i);
                            break;
                        }
                    }
                    total += count;
                    if(jaLeu==1){
                        break;
                    }
                }

                if(desconectou == 1){
                    printf("Client se desconectou repentinamente!\n");
                    //close(csock);
                    break;
                }
                
                //CONFERIR O CONTEUDO DA MENSAGEM. DAR CLOSE SE NECESSÁRIO
                if(strcmp(buf,"kill\n")==0){
                    printf("Pediu para matar o servidor\n");
                    matar_server = 1;
                    //Fecha conexão
                    close(csock);
                    break;
                }

                if(strcmp(buf, "logout\n")==0){
                    desconectou==1;
                    printf("Client fez logout!");
                    close(csock);
                    break;
                }
                
                //Usar fputs ao invés do printf
                fputs(buf, stdout);
                printf("[msg] %s, %d bytes: \'%s\'", caddrstr, (int)count, buf);

                //Envia uma resposta
                //sprintf(buf, "remote endpoint: %.500s\n", caddrstr);
                //printf("Aqui 1 SV\n");
                count = send(csock, buf, strlen(buf) + 1, 0);
                //printf("Aqui 2 SV\n");
                //Se não enviar o número certo de dados
                if (count != strlen(buf) + 1) {
                    logexit("send");
                }
                //printf("Aqui 3 SV\n");
            }
        }else{
            break;
        }
    }
    exit(EXIT_SUCCESS);
}